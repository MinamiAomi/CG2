#define NOMINMAX
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <combaseapi.h>

#include <cstdint>
#include <cassert>
#include <vector>

#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_impl_dx12.h"
#include "Externals/ImGui/imgui_impl_win32.h"
#include "Externals/DirectXTex/DirectXTex.h"
#include "Externals/DirectXTex/d3dx12.h"

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
struct Transform2D {
    Vector2 scale;
    float rotate;
    Vector2 translate;
};

struct VertexData {
    Vector4 position;
    Vector2 texcoord;
    Vector3 normal;
};

struct TransformationConstantData {
    Matrix4x4 wvp;
    Matrix4x4 world;
};

struct MaterialConstantData {
    Vector4 color;
    int32_t enableLighting;
    int32_t padding[3];
    Matrix4x4 uvTransform;
};

struct DirectionalLightConstantData {
    Vector4 color;
    Vector3 direction;
    float intensity;
};

struct GPUResource {
    ID3D12Resource* resource{ nullptr };
    D3D12_RESOURCE_STATES currentState{ D3D12_RESOURCE_STATE_COMMON };

    D3D12_RESOURCE_BARRIER TransitionBarrier(D3D12_RESOURCE_STATES nextState);
};

struct VertexResource : public GPUResource {
    uint32_t strideSize{};

    D3D12_VERTEX_BUFFER_VIEW View();
};

struct IndexResource : public GPUResource {

    D3D12_INDEX_BUFFER_VIEW View();
};

struct ConstantResource : public GPUResource {
    void* mappedData{ nullptr };
};

struct TextureResource : public GPUResource {
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
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
[[nodiscard]]
ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);
D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);

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
    const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

    ID3D12CommandQueue* commandQueue{ nullptr };
    ID3D12CommandAllocator* commandAllocator{ nullptr };
    ID3D12GraphicsCommandList* commandList{ nullptr };
    ID3D12Fence* fence{ nullptr };
    uint64_t fenceValue{ 0 };
    HANDLE fenceEvent{ nullptr };
    auto SubmitCommandList = [&]() {
        // コマンドリストの内容を確定
        HRESULT hr = commandList->Close();
        assert(SUCCEEDED(hr));
        // GPUにコマンドリストの実行を行わせる
        ID3D12CommandList* commandLists[] = { commandList };
        commandQueue->ExecuteCommandLists(1, commandLists);
        // Fenceの値を更新
        fenceValue++;
        // GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
        hr = commandQueue->Signal(fence, fenceValue);
        assert(SUCCEEDED(hr));
    };
    auto WaitForGpu = [&]() {
        // Fenceの値が指定したSignal値にたどり着いているか確認する
           // GetCompletedValueの初期値はFence作成時に渡した初期値
        if (fence->GetCompletedValue() < fenceValue) {
            // 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
            fence->SetEventOnCompletion(fenceValue, fenceEvent);
            // イベントを待つ
            WaitForSingleObject(fenceEvent, INFINITE);
        }
    };
    auto ResetCommandList = [&]() {
        // 次フレーム用のコマンドリストを準備
        HRESULT hr = commandAllocator->Reset();
        assert(SUCCEEDED(hr));
        hr = commandList->Reset(commandAllocator, nullptr);
        assert(SUCCEEDED(hr));
    };
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
            rtvHandle[i] = GetCPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, i);
            // RTVを生成
            device->CreateRenderTargetView(swapChainResource[i].resource, &rtvDesc, rtvHandle[i]);
            swapChainResource[i].currentState = D3D12_RESOURCE_STATE_PRESENT;
        }
    }
    ID3D12Resource* depthStencilResource{ nullptr };
    ID3D12DescriptorHeap* dsvDescriptorHeap{ nullptr };
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
    {
        depthStencilResource = CreateDepthStencilTextureResource(device, int32_t(kClientWidth), int32_t(kClientHeight));
        dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvHandle = GetCPUDescriptorHandle(dsvDescriptorHeap, descriptorSizeDSV, 0);
        device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvHandle);
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
            GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 0),
            GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 0));
    }

    ShaderCompiler shaderCompiler;
    shaderCompiler.Initalize();

    ID3D12RootSignature* rootSignature{ nullptr };
    ID3D12PipelineState* pipelineState{ nullptr };
    {
        HRESULT hr = S_FALSE;

        D3D12_ROOT_PARAMETER rootParameters[4] = {};
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

        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
        rootParameters[3].Descriptor.ShaderRegister = 1;

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
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};

        inputElementDescs[0].SemanticName = "POSITION";
        inputElementDescs[0].SemanticIndex = 0;
        inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        inputElementDescs[1].SemanticName = "TEXCOORD";
        inputElementDescs[1].SemanticIndex = 0;
        inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        inputElementDescs[2].SemanticName = "NORMAL";
        inputElementDescs[2].SemanticIndex = 0;
        inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
        inputLayoutDesc.pInputElementDescs = inputElementDescs;
        inputLayoutDesc.NumElements = _countof(inputElementDescs);

        // ブレンドステート
        D3D12_BLEND_DESC blendDesc{};
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

        D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

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
        graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
        graphicsPipelineStateDesc.NumRenderTargets = 1;
        graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
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

    Transform cameraTransform{ {1.0f,1.0f,1.0f}, {} ,{0.0f,0.0f,-10.0f} };
    ConstantResource directionalLightResource;
    DirectionalLightConstantData directionalLightConstantData{};
    {
        directionalLightConstantData.color = { 1.0f,1.0f,1.0f,1.0f };
        directionalLightConstantData.direction = { 0.0f,-1.0f,0.0f };
        directionalLightConstantData.intensity = 1.0f;
        directionalLightResource.resource = CreateBufferResource(device, sizeof(directionalLightConstantData));
        directionalLightResource.resource->Map(0, nullptr, &directionalLightResource.mappedData);
        memcpy(directionalLightResource.mappedData, &directionalLightConstantData, sizeof(directionalLightConstantData));
    }

    Transform transformSphere{ {1.0f,1.0f,1.0f}, {}, {} };
    Vector4 colorSphere{ 1.0f,1.0f,1.0f,1.0f };
    Transform2D uvTransformSphere{ {1.0f,1.0f}, 0.0f ,{} };
    VertexResource vertexResourceSphere;
    IndexResource indexResourceSphere;
    ConstantResource materialResourceSphere;
    ConstantResource transformationMatrixResourceSphere;
    {
        // 球を作成
        std::vector<VertexData> vertices;
        std::vector<uint32_t> indices;
        {
            const int32_t kSubdivision = 16;

            const float kLonEvery = Math::TwoPi / float(kSubdivision);
            const float kLatEvery = Math::Pi / float(kSubdivision);
            const size_t kLatVertexCount = size_t(kSubdivision + 1);
            const size_t kLonVertexCount = size_t(kSubdivision + 1);
            const size_t kVertexCount = kLonVertexCount * kLatVertexCount;

            vertices.resize(kVertexCount);

            auto CalcPosition = [](float lat, float lon) {
                return Vector4{
                    .x{ std::cos(lat) * std::cos(lon) },
                    .y{ std::sin(lat) },
                    .z{ std::cos(lat) * std::sin(lon) },
                    .w{ 1.0f }
                };
            };
            auto CalcTexcoord = [](size_t latIndex, size_t lonIndex) {
                return Vector2{
                    .x{ float(lonIndex) / float(kSubdivision) },
                    .y{ 1.0f - float(latIndex) / float(kSubdivision) },
                };
            };


            for (size_t latIndex = 0; latIndex < kLonVertexCount; ++latIndex) {
                float lat = -Math::HalfPi + kLatEvery * latIndex;

                for (size_t lonIndex = 0; lonIndex < kLatVertexCount; ++lonIndex) {
                    float lon = lonIndex * kLonEvery;

                    size_t vertexIndex = latIndex * kLatVertexCount + lonIndex;
                    vertices[vertexIndex].position = CalcPosition(lat, lon);
                    vertices[vertexIndex].texcoord = CalcTexcoord(latIndex, lonIndex);
                    vertices[vertexIndex].normal = ToVector3(vertices[vertexIndex].position);
                }
            }

            const size_t kIndexCount = size_t(kSubdivision * kSubdivision) * 6;
            indices.resize(kIndexCount);

            for (uint32_t i = 0; i < kSubdivision; ++i) {
                uint32_t y0 = i * kLatVertexCount;
                uint32_t y1 = (i + 1) * uint32_t(kLatVertexCount);

                for (uint32_t j = 0; j < kSubdivision; ++j) {
                    uint32_t index0 = y0 + j;
                    uint32_t index1 = y1 + j;
                    uint32_t index2 = y0 + j + 1;
                    uint32_t index3 = y1 + j + 1;

                    uint32_t indexIndex = (i * kSubdivision + j) * 6;
                    indices[indexIndex++] = index0;
                    indices[indexIndex++] = index1;
                    indices[indexIndex++] = index2;
                    indices[indexIndex++] = index1;
                    indices[indexIndex++] = index3;
                    indices[indexIndex++] = index2;
                }
            }
        }

        vertexResourceSphere.resource = CreateBufferResource(device, sizeof(vertices[0]) * vertices.size());
        vertexResourceSphere.strideSize = sizeof(vertices[0]);
        VertexData* vertexData = nullptr;
        vertexResourceSphere.resource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
        memcpy(vertexData, vertices.data(), sizeof(vertices[0]) * vertices.size());
        vertexResourceSphere.resource->Unmap(0, nullptr);

        indexResourceSphere.resource = CreateBufferResource(device, sizeof(indices[0]) * indices.size());
        uint32_t* indexData = nullptr;
        indexResourceSphere.resource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
        memcpy(indexData, indices.data(), sizeof(indices[0]) * indices.size());
        indexResourceSphere.resource->Unmap(0, nullptr);

        MaterialConstantData materialData{};
        materialData.color = colorSphere;
        materialData.enableLighting = true;
        materialData.uvTransform = MakeAffineMatrix(ToVector3(uvTransformSphere.scale, 1.0f), { 0.0f,0.0f,uvTransformSphere.rotate }, ToVector3(uvTransformSphere.translate, 0.0f));
        materialResourceSphere.resource = CreateBufferResource(device, sizeof(materialData));
        materialResourceSphere.resource->Map(0, nullptr, &materialResourceSphere.mappedData);
        memcpy(materialResourceSphere.mappedData, &materialData, sizeof(materialData));

        Matrix4x4 worldMatrix = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
        Matrix4x4 viewMatrix = Inverse(MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate));
        Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
        TransformationConstantData transformationData{ .wvp{worldMatrix * viewMatrix * projectionMatrix}, .world{worldMatrix} };
        transformationMatrixResourceSphere.resource = CreateBufferResource(device, sizeof(transformationData));
        transformationMatrixResourceSphere.resource->Map(0, nullptr, &transformationMatrixResourceSphere.mappedData);
        memcpy(transformationMatrixResourceSphere.mappedData, &transformationData, sizeof(transformationData));
    }

    Transform transformSprite{ {1.0f,1.0f,1.0f}, {}, {} };
    Vector4 colorSprite{ 1.0f,1.0f,1.0f,1.0f };
    Transform2D uvTransformSprite{ {1.0f,1.0f}, 0.0f ,{} };
    VertexResource vertexResourceSprite;
    IndexResource indexResourceSprite;
    ConstantResource transformationMatrixResourceSprite;
    ConstantResource materialResourceSprite;
    {
        VertexData vertices[]{
            {.position{   0.0f, 360.0f, 0.0f, 1.0f }, .texcoord{ 0.0f, 1.0f }, .normal{ 0.0f, 0.0f, -1.0f } },
            {.position{   0.0f,   0.0f, 0.0f, 1.0f }, .texcoord{ 0.0f, 0.0f }, .normal{ 0.0f, 0.0f, -1.0f } },
            {.position{ 640.0f, 360.0f, 0.0f, 1.0f }, .texcoord{ 1.0f, 1.0f }, .normal{ 0.0f, 0.0f, -1.0f } },
            {.position{ 640.0f,   0.0f, 0.0f, 1.0f }, .texcoord{ 1.0f, 0.0f }, .normal{ 0.0f, 0.0f, -1.0f } },
        };
        vertexResourceSprite.resource = CreateBufferResource(device, sizeof(vertices));
        vertexResourceSprite.strideSize = sizeof(vertices[0]);

        VertexData* vertexData = nullptr;
        vertexResourceSprite.resource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
        memcpy(vertexData, vertices, sizeof(vertices));
        vertexResourceSprite.resource->Unmap(0, nullptr);

        uint32_t indices[]{
            0, 1, 2,
            1, 3, 2
        };
        indexResourceSprite.resource = CreateBufferResource(device, sizeof(indices));

        uint32_t* indexData = nullptr;
        indexResourceSprite.resource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
        memcpy(indexData, indices, sizeof(indices));
        indexResourceSprite.resource->Unmap(0, nullptr);

        MaterialConstantData materialData{};
        materialData.color = colorSprite;
        materialData.enableLighting = false;
        materialData.uvTransform = MakeAffineMatrix(ToVector3(uvTransformSprite.scale, 1.0f), { 0.0f,0.0f,uvTransformSprite.rotate }, ToVector3(uvTransformSprite.translate, 0.0f));
        materialResourceSprite.resource = CreateBufferResource(device, sizeof(materialData));
        materialResourceSprite.resource->Map(0, nullptr, &materialResourceSprite.mappedData);
        memcpy(materialResourceSprite.mappedData, &materialData, sizeof(materialData));

        TransformationConstantData transformationConstantData{ .wvp{ MakeIdentityMatrix() }, .world{ MakeIdentityMatrix() } };
        transformationMatrixResourceSprite.resource = CreateBufferResource(device, sizeof(transformationConstantData));
        transformationMatrixResourceSprite.resource->Map(0, nullptr, &transformationMatrixResourceSprite.mappedData);
        memcpy(transformationMatrixResourceSprite.mappedData, &transformationConstantData, sizeof(transformationConstantData));
    }

    TextureResource textureResource{ nullptr };
    {
        auto mipImages = LoadTexture("resources/uvChecker.png");
        const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
        textureResource.resource = CreateTextureResource(device, metadata);

        ID3D12Resource* intermediateResource = UploadTextureData(textureResource.resource, mipImages, device, commandList);
        SubmitCommandList();
        WaitForGpu();
        ResetCommandList();
        intermediateResource->Release();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = metadata.format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

        textureResource.cpuHandle = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
        textureResource.gpuHandle = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);

        device->CreateShaderResourceView(textureResource.resource, &srvDesc, textureResource.cpuHandle);
    }

    TextureResource textureResource2{ nullptr };
    {
        auto mipImages = LoadTexture("resources/monsterBall.png");
        const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
        textureResource2.resource = CreateTextureResource(device, metadata);

        ID3D12Resource* intermediateResource = UploadTextureData(textureResource2.resource, mipImages, device, commandList);
        SubmitCommandList();
        WaitForGpu();
        ResetCommandList();
        intermediateResource->Release();

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = metadata.format;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

        textureResource2.cpuHandle = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
        textureResource2.gpuHandle = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);

        device->CreateShaderResourceView(textureResource2.resource, &srvDesc, textureResource2.cpuHandle);
    }

    bool showObject = true;
    bool showSprite = true;
    bool useMonsterBall = false;

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
                commandList->OMSetRenderTargets(1, &rtvHandle[backBufferIndex], false, &dsvHandle);
                // 指定した色で画面全体をクリア
                float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
                commandList->ClearRenderTargetView(rtvHandle[backBufferIndex], clearColor, 0, nullptr);
                commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

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
                ImGui::SetNextWindowPos({ kClientWidth - 300, 0.0f }, ImGuiCond_Once);
                ImGui::SetNextWindowSize({ 300, 200.0f }, ImGuiCond_Once);
                ImGui::Begin("Window");
                if (ImGui::Checkbox("Show objects?", &showObject)) {

                }
                ImGui::Checkbox("Show sprites?", &showSprite);
                ImGui::Checkbox("Use MonsterBall?", &useMonsterBall);
                if (ImGui::TreeNode("Camera")) {
                    ImGui::DragFloat3("Position", &cameraTransform.translate.x, 0.01f);
                    Vector3 deg = cameraTransform.rotate * Math::ToDegree;
                    ImGui::DragFloat3("Rotate", &deg.x, 0.1f, -360.0f, 360.0f);
                    cameraTransform.rotate = deg * Math::ToRadian;
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("DirectionalLight")) {
                    ImGui::ColorEdit4("Color", &directionalLightConstantData.color.x);
                    ImGui::DragFloat3("Direction", &directionalLightConstantData.direction.x, 0.01f);
                    directionalLightConstantData.direction = Normalize(directionalLightConstantData.direction);
                    ImGui::DragFloat("Intensity", &directionalLightConstantData.intensity, 0.01f);
                    memcpy(directionalLightResource.mappedData, &directionalLightConstantData, sizeof(directionalLightConstantData));
                    ImGui::TreePop();
                }
                ImGui::End();

                if (showObject) {
                    ImGui::Begin("Object");
                    if (ImGui::TreeNode("Transform")) {
                        ImGui::DragFloat3("Translate", &transformSphere.translate.x, 0.01f);
                        Vector3 deg = transformSphere.rotate * Math::ToDegree;
                        ImGui::DragFloat3("Rotate", &deg.x, 0.1f, -360.0f, 360.0f);
                        transformSphere.rotate = deg * Math::ToRadian;
                        ImGui::DragFloat3("Scale", &transformSphere.scale.x, 0.01f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Material")) {
                        ImGui::ColorEdit4("Color", &colorSphere.x);
                        if (ImGui::TreeNode("UVTransform")) {
                            ImGui::DragFloat2("Translate", &uvTransformSphere.translate.x, 0.01f);
                            float deg = uvTransformSphere.rotate * Math::ToDegree;
                            ImGui::DragFloat("Rotate", &deg, 0.1f, -360.0f, 360.0f);
                            uvTransformSphere.rotate = deg * Math::ToRadian;
                            ImGui::DragFloat2("Scale", &uvTransformSphere.scale.x, 0.01f);
                            ImGui::TreePop();
                        }
                        ImGui::TreePop();
                    }
                    ImGui::End();
                }

                if (showSprite) {
                    ImGui::Begin("Sprite");
                    if (ImGui::TreeNode("Transform")) {
                        ImGui::DragFloat3("Translate", &transformSprite.translate.x, 0.01f);
                        Vector3 deg = transformSprite.rotate * Math::ToDegree;
                        ImGui::DragFloat3("Rotate", &deg.x, 0.1f, -360.0f, 360.0f);
                        transformSprite.rotate = deg * Math::ToRadian;
                        ImGui::DragFloat3("Scale", &transformSprite.scale.x, 0.01f);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Material")) {
                        ImGui::ColorEdit4("Color", &colorSprite.x);
                        if (ImGui::TreeNode("UVTransform")) {
                            ImGui::DragFloat2("Translate", &uvTransformSprite.translate.x, 0.01f);
                            float deg = uvTransformSprite.rotate * Math::ToDegree;
                            ImGui::DragFloat("Rotate", &deg, 0.1f, -360.0f, 360.0f);
                            uvTransformSprite.rotate = deg * Math::ToRadian;
                            ImGui::DragFloat2("Scale", &uvTransformSprite.scale.x, 0.01f);
                            ImGui::TreePop();
                        }
                        ImGui::TreePop();
                    }
                    ImGui::End();
                }

                commandList->SetGraphicsRootSignature(rootSignature);
                commandList->SetPipelineState(pipelineState);
                commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource.resource->GetGPUVirtualAddress());
                if (showObject) {
                    transformSphere.rotate.y += 0.01f;
                    Matrix4x4 worldMatrix = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
                    Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
                    Matrix4x4 viewMatrix = Inverse(cameraMatrix);
                    Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
                    Matrix4x4 wvp = worldMatrix * viewMatrix * projectionMatrix;
                    TransformationConstantData transformationConstantData{ .wvp{wvp}, .world{worldMatrix} };
                    memcpy(transformationMatrixResourceSphere.mappedData, &transformationConstantData, sizeof(transformationConstantData));

                    MaterialConstantData materialData{};
                    materialData.color = colorSphere;
                    materialData.enableLighting = true;
                    materialData.uvTransform = MakeAffineMatrix(ToVector3(uvTransformSphere.scale, 1.0f), { 0.0f,0.0f,uvTransformSphere.rotate }, ToVector3(uvTransformSphere.translate, 0.0f));
                    memcpy(materialResourceSphere.mappedData, &materialData, sizeof(materialData));

                    auto vertexBufferView = vertexResourceSphere.View();
                    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
                    auto indexBufferView = indexResourceSphere.View();
                    commandList->IASetIndexBuffer(&indexBufferView);
                    commandList->SetGraphicsRootConstantBufferView(0, transformationMatrixResourceSphere.resource->GetGPUVirtualAddress());
                    commandList->SetGraphicsRootConstantBufferView(1, materialResourceSphere.resource->GetGPUVirtualAddress());
                    commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureResource2.gpuHandle : textureResource.gpuHandle);
                    uint32_t indexCount = indexBufferView.SizeInBytes / sizeof(uint32_t);
                    commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
                }
                if (showSprite) {
                    Matrix4x4 worldMatrix = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
                    Matrix4x4 viewMatrix = MakeIdentityMatrix();
                    Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
                    Matrix4x4 wvp = worldMatrix * viewMatrix * projectionMatrix;
                    TransformationConstantData transformationConstantData{ .wvp{wvp}, .world{worldMatrix} };
                    memcpy(transformationMatrixResourceSprite.mappedData, &transformationConstantData, sizeof(transformationConstantData));

                    MaterialConstantData materialData{};
                    materialData.color = colorSprite;
                    materialData.enableLighting = false;
                    materialData.uvTransform = MakeAffineMatrix(ToVector3(uvTransformSprite.scale, 1.0f), { 0.0f,0.0f,uvTransformSprite.rotate }, ToVector3(uvTransformSprite.translate, 0.0f));
                    memcpy(materialResourceSprite.mappedData, &materialData, sizeof(materialData));

                    auto vertexBufferView = vertexResourceSprite.View();
                    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
                    auto indexBufferView = indexResourceSprite.View();
                    commandList->IASetIndexBuffer(&indexBufferView);
                    commandList->SetGraphicsRootConstantBufferView(0, transformationMatrixResourceSprite.resource->GetGPUVirtualAddress());
                    commandList->SetGraphicsRootConstantBufferView(1, materialResourceSprite.resource->GetGPUVirtualAddress());
                    commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureResource2.gpuHandle : textureResource.gpuHandle);
                    uint32_t indexCount = indexBufferView.SizeInBytes / sizeof(uint32_t);
                    commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
                }


                //-----------------------------------------------------------------------------------------//


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

        indexResourceSprite.resource->Release();
        transformationMatrixResourceSprite.resource->Release();
        materialResourceSprite.resource->Release();
        vertexResourceSprite.resource->Release();
        textureResource2.resource->Release();
        textureResource.resource->Release();
        transformationMatrixResourceSphere.resource->Release();
        materialResourceSphere.resource->Release();
        indexResourceSphere.resource->Release();
        vertexResourceSphere.resource->Release();
        directionalLightResource.resource->Release();
        pipelineState->Release();
        rootSignature->Release();
        srvDescriptorHeap->Release();
        dsvDescriptorHeap->Release();
        depthStencilResource->Release();
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

D3D12_VERTEX_BUFFER_VIEW VertexResource::View() {
    assert(resource);
    D3D12_VERTEX_BUFFER_VIEW view{};
    view.BufferLocation = resource->GetGPUVirtualAddress();
    view.SizeInBytes = static_cast<uint32_t>(resource->GetDesc().Width);
    view.StrideInBytes = strideSize;
    return view;
}

D3D12_INDEX_BUFFER_VIEW IndexResource::View() {
    assert(resource);
    D3D12_INDEX_BUFFER_VIEW view{};
    view.BufferLocation = resource->GetGPUVirtualAddress();
    view.SizeInBytes = static_cast<uint32_t>(resource->GetDesc().Width);
    view.Format = DXGI_FORMAT_R32_UINT;
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
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));
    return resource;
}

ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
    ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
    UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    commandList->ResourceBarrier(1, &barrier);
    return intermediateResource;
}

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.MipLevels = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    ID3D12Resource* resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue,
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));
    return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
    D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
    handleCPU.ptr += (descriptorSize * index);
    return handleCPU;
}
D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
    D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
    handleGPU.ptr += (descriptorSize * index);
    return handleGPU;
}

