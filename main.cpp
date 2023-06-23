#define NOMINMAX
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <combaseapi.h>

#include <cstdint>
#include <cassert>

#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_impl_dx12.h"
#include "Externals/ImGui/imgui_impl_win32.h"
#include "Externals/DirectXTex/DirectXTex.h"

#include "StringUtils.h"
#include "Math/MathUtils.h"
#include "StringUtils.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include <filesystem>

// 定数
constexpr uint32_t kSwapChainBufferCount = 2;
// クライアント領域サイズ
const uint32_t kClientWidth = 1280;
const uint32_t kClientHeight = 720;

struct Transform {
    Vector3 scale;
    Vector3 rotate;
    Vector3 translate;
};

struct VertexData {
    Vector4 position;
    Vector2 texcoord;
};

struct TransformConstantData {
    Matrix4x4 wvp;
    Matrix4x4 world;
};

struct GPUResource {
    ID3D12Resource* resource{ nullptr };
    D3D12_RESOURCE_STATES currentState{ D3D12_RESOURCE_STATE_COMMON };

    D3D12_RESOURCE_BARRIER TransitionBarrier(D3D12_RESOURCE_STATES nextState);
};

struct VertexBuffer : public GPUResource {
    uint32_t strideSize{};

    D3D12_VERTEX_BUFFER_VIEW View();
};

struct ConstantBuffer : public GPUResource {
    void* mapData{ nullptr };
};

class ShaderCompiler {
public:
    void Initalize();
    IDxcBlob* Compile(const std::wstring& filePath, const wchar_t* profile);

private:
    IDxcUtils* dxcUtils_{ nullptr };
    IDxcCompiler3* dxcCompiler_{ nullptr };
    IDxcIncludeHandler* includeHandler_{ nullptr };
};

// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
    // メッセージに対してゲーム固有の処理を行う
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t descriptorCount, bool shaderVisible);
DirectX::ScratchImage LoadTexture(const std::string& filePath);
ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // 出力ウィンドウに文字出力
    Log("Hello,DirectX!\n");

    if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) {
        assert(false);
    }


    HWND hwnd{ nullptr };
    {
        // ウィンドウクラスを生成
        WNDCLASS wc{};
        wc.lpfnWndProc = WindowProc;	// ウィンドウプロシージャ
        wc.lpszClassName = L"CG2WindowClass";	// ウィンドウクラス名
        wc.hInstance = GetModuleHandle(nullptr);	// インスタンスハンドル
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);	// カーソル
        RegisterClass(&wc);	// ウィンドウクラスを登録

        // ウィンドウサイズを表す構造体にクライアント領域を入れる
        RECT wrc{ 0,0,static_cast<LONG>(kClientWidth),static_cast<LONG>(kClientHeight) };
        // クライアント領域を元に実際のサイズにwrcを変更してもらう
        AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

        auto wname = ConvertString("CG2");
        // ウィンドウの生成
        hwnd = CreateWindow(
            wc.lpszClassName,		// 利用するクラス名
            wname.c_str(),				// タイトルバーの文字
            WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
            CW_USEDEFAULT,			// 表示X座標（WindowsOSに任せる）
            CW_USEDEFAULT,			// 表示Y座標（WindowsOSに任せる）
            wrc.right - wrc.left,	// ウィンドウ横幅
            wrc.bottom - wrc.top,	// ウィンドウ縦幅
            nullptr,				// 親ウィンドウハンドル
            nullptr,				// メニューハンドル
            wc.hInstance,			// インスタンスハンドル
            nullptr);				// オプション

        ShowWindow(hwnd, SW_SHOW);
    }

    IDXGIFactory7* dxgiFactory{ nullptr };
    ID3D12Device* device{ nullptr };
    {
#ifdef _DEBUG	
        // デバッグ時のみ
        ID3D12Debug1* debugController = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            // デバッグレイヤーを有効化する
            debugController->EnableDebugLayer();
            // さらにGPU側でもチェックを行えるようにする
            debugController->SetEnableGPUBasedValidation(TRUE);
            debugController->Release();
        }
#endif
        HRESULT hr = S_FALSE;

        // DXGIファクトリーの生成
        hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
        assert(SUCCEEDED(hr));

        // 使用するアダプタ用の変数
        IDXGIAdapter4* useAdapter = nullptr;

        // 良い順にアダプターを頼む
        for (uint32_t i = 0;
            dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
            ++i) {
            // アダプター情報を取得
            DXGI_ADAPTER_DESC3 adapterDesc{};
            hr = useAdapter->GetDesc3(&adapterDesc);
            assert(SUCCEEDED(hr));
            // ソフトウェアアダプタでなければ採用
            if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
                // 採用したアダプタ情報を出力
                Log(std::format(L"Use Adapter:{}\n", adapterDesc.Description));
                break;
            }
            useAdapter = nullptr; // ソフトウェアアダプタは見なかったことにする
        }
        assert(useAdapter != nullptr);


        // 機能レベルとログ出力用の文字列
        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
        };
        const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
        // 高い順に生成できるか試していく
        for (size_t i = 0; i < _countof(featureLevels); ++i) {
            // 採用したアダプターデバイスを生成
            hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));
            // 指定した機能レベルでデバイスが生成できたかを確認
            if (SUCCEEDED(hr)) {
                // 生成できたのでログ出力を行ってループを抜ける
                Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
                break;
            }
        }
        assert(device != nullptr);
        useAdapter->Release();
        Log("Complete create D3D12Device!!!\n"); // 初期化完了のログを出す

#ifdef _DEBUG
        // デバッグ時のみ
        ID3D12InfoQueue* infoQueue = nullptr;
        if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
            // やばいエラーの時に止まる
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
            // エラーの時に止まる
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
            // 警告時に止まる
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
            // 抑制するメッセージのID
            D3D12_MESSAGE_ID denyIds[] = {
                D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
            };
            // 抑制するレベル
            D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
            D3D12_INFO_QUEUE_FILTER filter{};
            filter.DenyList.NumIDs = _countof(denyIds);
            filter.DenyList.pIDList = denyIds;
            filter.DenyList.NumSeverities = _countof(severities);
            filter.DenyList.pSeverityList = severities;
            // 指定したメッセージの表示を抑制する
            infoQueue->PushStorageFilter(&filter);
            // 解放
            infoQueue->Release();
        }
#endif
    }

    ID3D12CommandQueue* commandQueue{ nullptr };
    ID3D12CommandAllocator* commandAllocator{ nullptr };
    ID3D12GraphicsCommandList* commandList{ nullptr };
    ID3D12Fence* fence{ nullptr };
    uint64_t fenceValue{ 0 };
    HANDLE fenceEvent{ nullptr };
    {
        HRESULT hr = S_FALSE;
        // コマンドキューを生成
        D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
        hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
        assert(SUCCEEDED(hr));

        // コマンドアロケータを生成
        hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
        assert(SUCCEEDED(hr));

        // コマンドリストを生成
        hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
        assert(SUCCEEDED(hr));

        // フェンスを生成
        hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        assert(SUCCEEDED(hr));

        fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        assert(fenceEvent != nullptr);
    }

    IDXGISwapChain4* swapChain{ nullptr };
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    ID3D12DescriptorHeap* rtvDescriptorHeap{ nullptr };
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
    GPUResource swapChainResource[kSwapChainBufferCount]{ nullptr };
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle[kSwapChainBufferCount]{};
    {
        WINDOWINFO winInfo{};
        winInfo.cbSize = sizeof(winInfo);
        if (!GetWindowInfo(hwnd, &winInfo)) {
            assert(false);
        }

        HRESULT hr = S_FALSE;
        // スワップチェーンの設定
        swapChainDesc.Width = static_cast<uint32_t>(winInfo.rcClient.right - winInfo.rcClient.left);		// 画面幅
        swapChainDesc.Height = static_cast<uint32_t>(winInfo.rcClient.bottom - winInfo.rcClient.top);	// 画面高
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色の形式
        swapChainDesc.SampleDesc.Count = 1;					// マルチサンプル市内
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画ターゲットとして利用する
        swapChainDesc.BufferCount = kSwapChainBufferCount;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// モニタに移したら、中身を破棄
        // スワップチェーンを生成
        hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
        assert(SUCCEEDED(hr));

        rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, kSwapChainBufferCount, false);
        // RTVの設定
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        // SwapChainResourceの生成とRTVの生成
        for (uint32_t i = 0; i < kSwapChainBufferCount; ++i) {
            // SwapChainからResourceを引っ張ってくる
            hr = swapChain->GetBuffer(i, IID_PPV_ARGS(&swapChainResource[i].resource));
            assert(SUCCEEDED(hr));
            // ディスクリプタの先頭を取得
            rtvHandle[i] = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
            // ディスクリプタハンドルをずらす
            rtvHandle[i].ptr += static_cast<size_t>(i) * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            // RTVを生成
            device->CreateRenderTargetView(swapChainResource[i].resource, &rtvDesc, rtvHandle[i]);
            swapChainResource[i].currentState = D3D12_RESOURCE_STATE_PRESENT;
        }
    }

    ID3D12DescriptorHeap* srvDescriptorHeap{ nullptr };
    {
        srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
    }
    // ImGuiの初期化
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX12_Init(
            device,
            swapChainDesc.BufferCount,
            rtvDesc.Format,
            srvDescriptorHeap,
            srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    }

    ShaderCompiler shaderCompiler;
    shaderCompiler.Initalize();

    ID3D12RootSignature* rootSignature{ nullptr };
    ID3D12PipelineState* pipelineState{ nullptr };
    {
        HRESULT hr = S_FALSE;

        D3D12_ROOT_PARAMETER rootParameters[3] = {};
        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
        rootParameters[0].Descriptor.ShaderRegister = 0;
       
        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[1].Descriptor.ShaderRegister = 0;

        D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
        descriptorRange[0].BaseShaderRegister = 0;
        descriptorRange[0].NumDescriptors = 1;
        descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
        rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

        D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
        staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;
        staticSampler[0].ShaderRegister = 0;
        staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        descriptionRootSignature.pParameters = rootParameters;
        descriptionRootSignature.NumParameters = _countof(rootParameters);
        descriptionRootSignature.pStaticSamplers = staticSampler;
        descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);

        ID3DBlob* signatureBlob = nullptr;
        ID3DBlob* errorBlob = nullptr;

        hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
        if (FAILED(hr)) {
            Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
            assert(false);
        }
        hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
        assert(SUCCEEDED(hr));


        // インプットレイアウト
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};

        inputElementDescs[0].SemanticName = "POSITION";
        inputElementDescs[0].SemanticIndex = 0;
        inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        
        inputElementDescs[1].SemanticName = "TEXCOORD";
        inputElementDescs[1].SemanticIndex = 0;
        inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        
        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
        inputLayoutDesc.pInputElementDescs = inputElementDescs;
        inputLayoutDesc.NumElements = _countof(inputElementDescs);

        // ブレンドステート
        D3D12_BLEND_DESC blendDesc{};
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        // ラスタライザステート
        D3D12_RASTERIZER_DESC rasterizerDesc{};
        rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

        // シェーダーをコンパイル
        IDxcBlob* vertexShaderBlob = shaderCompiler.Compile(L"Object3d.VS.hlsl", L"vs_6_0");
        assert(vertexShaderBlob != nullptr);
        IDxcBlob* pixelShaderBlob = shaderCompiler.Compile(L"Object3d.PS.hlsl", L"ps_6_0");
        assert(pixelShaderBlob != nullptr);

        // パイプライン生成
        D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
        graphicsPipelineStateDesc.pRootSignature = rootSignature;
        graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
        graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
        graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
        graphicsPipelineStateDesc.BlendState = blendDesc;
        graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
        graphicsPipelineStateDesc.NumRenderTargets = 1;
        graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        graphicsPipelineStateDesc.SampleDesc.Count = 1;
        graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

        hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineState));
        assert(SUCCEEDED(hr));

        signatureBlob->Release();
        if (errorBlob) {
            errorBlob->Release();
        }
        vertexShaderBlob->Release();
        pixelShaderBlob->Release();
    }

    VertexBuffer vertexBuffer;
    ConstantBuffer materialBuffer;
    Transform transform{ {1.0f,1.0f,1.0f}, {}, {} };
    ConstantBuffer wvpBuffer;
    Transform cameraTransform{ {1.0f,1.0f,1.0f}, {} ,{0.0f,0.0f,-5.0f} };
    {
        vertexBuffer.resource = CreateBufferResource(device, sizeof(VertexData) * 3);
        vertexBuffer.strideSize = sizeof(VertexData);

        VertexData* vertexData = nullptr;
        vertexBuffer.resource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
        vertexData[0].position = { -0.5f, -0.5f, 0.0f, 1.0f };
        vertexData[0].texcoord = { 0.0f, 1.0f };
        vertexData[1].position = { 0.0f,  0.5f, 0.0f, 1.0f };
        vertexData[1].texcoord = { 0.5f, 0.0f };
        vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
        vertexData[2].texcoord = { 1.0f, 1.0f };
        vertexBuffer.resource->Unmap(0, nullptr);

        materialBuffer.resource = CreateBufferResource(device, sizeof(Vector4));
        materialBuffer.resource->Map(0, nullptr, &materialBuffer.mapData);
        Vector4 materialData{ 1.0f,1.0f,0.0f,1.0f };
        memcpy(materialBuffer.mapData, &materialData, sizeof(materialData));

        wvpBuffer.resource = CreateBufferResource(device, sizeof(Matrix4x4));
        wvpBuffer.resource->Map(0, nullptr, &wvpBuffer.mapData);
        Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
        Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
        Matrix4x4 viewMatrix = Inverse(cameraMatrix);
        Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
        Matrix4x4 wvpData = worldMatrix * viewMatrix * projectionMatrix;
        memcpy(wvpBuffer.mapData, &wvpData, sizeof(wvpData));
    }

    ID3D12Resource* textureResource{ nullptr };
    D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU{};
    D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU{};
    {
        auto mipImages = LoadTexture("resources/uvChecker.png");
        const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
        textureResource = CreateTextureResource(device, metadata);
        UploadTextureData(textureResource, mipImages);


        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = metadata.format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

        textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
        textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

        textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);
    }

    {
        HRESULT hr = S_FALSE;
        MSG msg{};
        // ウィンドウの×ボタンがが押されるまでループ
        while (msg.message != WM_QUIT) {
            // Windowにメッセージが来てたら最優先で処理させる
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else {
                ImGui_ImplWin32_NewFrame();
                ImGui_ImplDX12_NewFrame();
                ImGui::NewFrame();

                // これから書き込むバックバッファインデックスを取得
                uint32_t backBufferIndex = swapChain->GetCurrentBackBufferIndex();
                // レンダ―ターゲットに遷移
                auto barrier = swapChainResource[backBufferIndex].TransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
                // TransitionBarrierを張る
                commandList->ResourceBarrier(1, &barrier);

                // 描画先のRTVを設定
                commandList->OMSetRenderTargets(1, &rtvHandle[backBufferIndex], false, nullptr);
                // 指定した色で画面全体をクリア
                float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
                commandList->ClearRenderTargetView(rtvHandle[backBufferIndex], clearColor, 0, nullptr);

                D3D12_VIEWPORT viewport{};
                viewport.Width = static_cast<float>(kClientWidth);
                viewport.Height = static_cast<float>(kClientHeight);
                viewport.TopLeftX = 0.0f;
                viewport.TopLeftY = 0.0f;
                viewport.MinDepth = 0.0f;
                viewport.MaxDepth = 1.0f;
                commandList->RSSetViewports(1, &viewport);

                D3D12_RECT scissorRect{};
                scissorRect.left = 0;
                scissorRect.right = kClientWidth;
                scissorRect.top = 0;
                scissorRect.bottom = kClientHeight;
                commandList->RSSetScissorRects(1, &scissorRect);

                ID3D12DescriptorHeap* ppHeaps[] = { srvDescriptorHeap };
                commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

                //-----------------------------------------------------------------------------------------//

                transform.rotate.y += 0.03f;
                Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
                Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
                Matrix4x4 viewMatrix = Inverse(cameraMatrix);
                Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
                Matrix4x4 wvpData = worldMatrix * viewMatrix * projectionMatrix;
                memcpy(wvpBuffer.mapData, &wvpData, sizeof(wvpData));

                commandList->SetGraphicsRootSignature(rootSignature);
                commandList->SetPipelineState(pipelineState);
                commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                auto vertexBufferView = vertexBuffer.View();
                commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
                commandList->SetGraphicsRootConstantBufferView(0, wvpBuffer.resource->GetGPUVirtualAddress());
                commandList->SetGraphicsRootConstantBufferView(1, materialBuffer.resource->GetGPUVirtualAddress());
                commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
                commandList->DrawInstanced(3, 1, 0, 0);

                //-----------------------------------------------------------------------------------------//

                ImGui::Begin("Window");
                ImGui::End();

                ImGui::Render();
                ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

                // RenderTargetからPresentへ
                barrier = swapChainResource[backBufferIndex].TransitionBarrier(D3D12_RESOURCE_STATE_PRESENT);
                // TransitionBarrierを張る
                commandList->ResourceBarrier(1, &barrier);

                // コマンドリストの内容を確定
                hr = commandList->Close();
                assert(SUCCEEDED(hr));
                // GPUにコマンドリストの実行を行わせる
                ID3D12CommandList* commandLists[] = { commandList };
                commandQueue->ExecuteCommandLists(1, commandLists);
                // GPUとOSに画面交換を行うよう通知する
                swapChain->Present(1, 0);
                // Fenceの値を更新
                fenceValue++;
                // GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
                hr = commandQueue->Signal(fence, fenceValue);
                assert(SUCCEEDED(hr));
                // Fenceの値が指定したSignal値にたどり着いているか確認する
                // GetCompletedValueの初期値はFence作成時に渡した初期値
                if (fence->GetCompletedValue() < fenceValue) {
                    // 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
                    fence->SetEventOnCompletion(fenceValue, fenceEvent);
                    // イベントを待つ
                    WaitForSingleObject(fenceEvent, INFINITE);
                }
                // 次フレーム用のコマンドリストを準備
                hr = commandAllocator->Reset();
                assert(SUCCEEDED(hr));
                hr = commandList->Reset(commandAllocator, nullptr);
                assert(SUCCEEDED(hr));
            }
        }
    }

    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        textureResource->Release();
        wvpBuffer.resource->Release();
        materialBuffer.resource->Release();
        vertexBuffer.resource->Release();
        pipelineState->Release();
        rootSignature->Release();
        srvDescriptorHeap->Release();
        swapChainResource[0].resource->Release();
        swapChainResource[1].resource->Release();
        rtvDescriptorHeap->Release();
        swapChain->Release();
        CloseHandle(fenceEvent);
        fence->Release();
        commandList->Release();
        commandAllocator->Release();
        commandQueue->Release();
        device->Release();
        dxgiFactory->Release();
        CloseWindow(hwnd);

        IDXGIDebug* debug = nullptr;
        if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
            debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
            debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
            debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
            debug->Release();
        }
    }

    CoUninitialize();

    return 0;
}

ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
    // アップロードヒープ
    D3D12_HEAP_PROPERTIES uploadHeapProperties{};
    uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    // 頂点バッファの設定
    D3D12_RESOURCE_DESC bufferDesc{};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = sizeInBytes;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    ID3D12Resource* result = nullptr;
    // 頂点バッファを生成
    HRESULT hr = S_FALSE;
    hr = device->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&result));
    assert(SUCCEEDED(hr));
    return result;
}

ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, uint32_t descriptorCount, bool shaderVisible) {
    D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
    descriptorHeapDesc.Type = heapType;
    descriptorHeapDesc.NumDescriptors = descriptorCount;
    descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    ID3D12DescriptorHeap* descriptorHeap{ nullptr };
    HRESULT hr = S_FALSE;
    hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
    assert(SUCCEEDED(hr));
    return descriptorHeap;
}

D3D12_RESOURCE_BARRIER GPUResource::TransitionBarrier(D3D12_RESOURCE_STATES nextState) {
    assert(resource);
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = resource;
    barrier.Transition.StateBefore = currentState;
    barrier.Transition.StateAfter = nextState;
    currentState = nextState;
    return barrier;
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::View() {
    assert(resource);
    D3D12_VERTEX_BUFFER_VIEW view{};
    view.BufferLocation = resource->GetGPUVirtualAddress();
    view.SizeInBytes = static_cast<uint32_t>(resource->GetDesc().Width);
    view.StrideInBytes = strideSize;
    return view;
}

void ShaderCompiler::Initalize() {
    HRESULT hr = S_FALSE;

    hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
    assert(SUCCEEDED(hr));

    hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
    assert(SUCCEEDED(hr));
}

IDxcBlob* ShaderCompiler::Compile(const std::wstring& filePath, const wchar_t* profile) {
    Log(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile));

    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr = S_FALSE;
    hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    assert(SUCCEEDED(hr));

    DxcBuffer shaderSourceBuffer{};
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;

    LPCWSTR arguments[] = {
        filePath.c_str(),
        L"-E", L"main",
        L"-T", profile,
        L"-Zi", L"-Qembed_debug",
        L"-Od",
        L"-Zpr"
    };

    IDxcResult* shaderResult = nullptr;
    hr = dxcCompiler_->Compile(
        &shaderSourceBuffer,
        arguments,
        _countof(arguments),
        includeHandler_,
        IID_PPV_ARGS(&shaderResult));
    assert(SUCCEEDED(hr));

    IDxcBlobUtf8* shaderError = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        Log(shaderError->GetStringPointer());
        assert(false);
    }

    IDxcBlob* shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));

    Log(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile));
    shaderSource->Release();
    shaderResult->Release();
    return shaderBlob;
}

DirectX::ScratchImage LoadTexture(const std::string& filePath) {
    DirectX::ScratchImage image{};
    std::wstring filePathW = ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    DirectX::ScratchImage mipImages{};
    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
    assert(SUCCEEDED(hr));

    return mipImages;
}

ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width);
    resourceDesc.Height = UINT(metadata.height);
    resourceDesc.MipLevels = UINT16(metadata.mipLevels);
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
    resourceDesc.Format = metadata.format;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
    heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
    heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

    ID3D12Resource* resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));
    return resource;
}

void UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages) {
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
    for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
        const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
        HRESULT hr = texture->WriteToSubresource(
            UINT(mipLevel),
            nullptr,
            img->pixels,
            UINT(img->rowPitch),
            UINT(img->slicePitch));
        assert(SUCCEEDED(hr));
    }
}
