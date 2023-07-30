#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <combaseapi.h>
#include <wrl.h>

#include <cstdint>
#include <cassert>
#include <memory>
#include <vector>


#include "Externals/ImGui/imgui.h"
#include "Externals/ImGui/imgui_impl_dx12.h"
#include "Externals/ImGui/imgui_impl_win32.h"
#include "Externals/DirectXTex/DirectXTex.h"
#include "Externals/DirectXTex/d3dx12.h"

#include "Math/MathUtils.h"

#include "DX12/DX12.h"

#include "Window.h"
#include "GraphicsEngine.h"
#include "ImGuiManager.h"
#include "ResourceManager.h"
#include "Model.h"


// 定数
constexpr uint32_t kSwapChainBufferCount = 2;
// クライアント領域サイズ
const uint32_t kClientWidth = 1280;
const uint32_t kClientHeight = 720;

//struct Transform {
//    Vector3 scale;
//    Vector3 rotate;
//    Vector3 translate;
//};
//struct Transform2D {
//    Vector2 scale;
//    float rotate;
//    Vector2 translate;
//};
//
//struct VertexData {
//    Vector4 position;
//    Vector2 texcoord;
//    Vector3 normal;
//};
//
//struct TransformationConstantData {
//    Matrix4x4 wvp;
//    Matrix4x4 world;
//};
//
//struct MaterialConstantData {
//    Vector4 color;
//    int32_t enableLighting;
//    int32_t padding[3];
//    Matrix4x4 uvTransform;
//};
//
//struct DirectionalLightConstantData {
//    Vector4 color;
//    Vector3 direction;
//    float intensity;
//};
//
//struct GPUResource {
//    Microsoft::WRL::ComPtr<ID3D12Resource> resource{ nullptr };
//    D3D12_RESOURCE_STATES currentState{ D3D12_RESOURCE_STATE_COMMON };
//
//    D3D12_RESOURCE_BARRIER TransitionBarrier(D3D12_RESOURCE_STATES nextState);
//};
//
//struct VertexResource : public GPUResource {
//    uint32_t strideSize{};
//
//    D3D12_VERTEX_BUFFER_VIEW View();
//};
//
//struct IndexResource : public GPUResource {
//
//    D3D12_INDEX_BUFFER_VIEW View();
//};
//
//struct ConstantResource : public GPUResource {
//    void* mappedData{ nullptr };
//};
//
//struct TextureResource : public GPUResource {
//    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle{};
//    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
//};



//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
//    CG::DX12::D3DResourceLeakChecker leakChecker;
//
//    CG::Window window;
//    window.Initialize("Game", 1280, 720);
//
//    if (FAILED(CoInitializeEx(0, COINIT_MULTITHREADED))) {
//        assert(false);
//    }
//
//    HWND hwnd{ nullptr };
//    {
//        // ウィンドウクラスを生成
//        WNDCLASS wc{};
//        wc.lpfnWndProc = WindowProc;	// ウィンドウプロシージャ
//        wc.lpszClassName = L"CG2WindowClass";	// ウィンドウクラス名
//        wc.hInstance = GetModuleHandle(nullptr);	// インスタンスハンドル
//        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);	// カーソル
//        RegisterClass(&wc);	// ウィンドウクラスを登録
//
//        // ウィンドウサイズを表す構造体にクライアント領域を入れる
//        RECT wrc{ 0,0,static_cast<LONG>(kClientWidth),static_cast<LONG>(kClientHeight) };
//        // クライアント領域を元に実際のサイズにwrcを変更してもらう
//        AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
//
//        auto wname = L"CG2";
//        // ウィンドウの生成
//        hwnd = CreateWindow(
//            wc.lpszClassName,		// 利用するクラス名
//            wname,				// タイトルバーの文字
//            WS_OVERLAPPEDWINDOW,	// よく見るウィンドウスタイル
//            CW_USEDEFAULT,			// 表示X座標（WindowsOSに任せる）
//            CW_USEDEFAULT,			// 表示Y座標（WindowsOSに任せる）
//            wrc.right - wrc.left,	// ウィンドウ横幅
//            wrc.bottom - wrc.top,	// ウィンドウ縦幅
//            nullptr,				// 親ウィンドウハンドル
//            nullptr,				// メニューハンドル
//            wc.hInstance,			// インスタンスハンドル
//            nullptr);				// オプション
//
//        ShowWindow(hwnd, SW_SHOW);
//    }
//
//    CG::DX12::Device device;
//    device.Initialize();
//
//    CG::DX12::CommandQueue commandQueue;
//    commandQueue.Initialize(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
//    CG::DX12::CommandList commandList;
//    commandList.Initialize(device, commandQueue);
//    CG::DX12::Fence fence;
//    fence.Initialize(device);
//
//    CG::DX12::DescriptorHeap rtvDescriptorHeap;
//    rtvDescriptorHeap.Initialize(device, 2, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//
//    CG::DX12::SwapChain swapChain;
//    swapChain.Initialize(hwnd, device, commandQueue, rtvDescriptorHeap, kClientWidth, kClientHeight);
//
//    CG::DX12::Resource resource;
//    resource.InitializeForBuffer(device, 256);
//
//    CG::DX12::RootSignatureDesc rsDesc;
//    rsDesc.AddDescriptor(CG::DX12::DescriptorType::CBV, 0, CG::DX12::ShaderVisibility::Vertex);
//    rsDesc.AddDescriptor(CG::DX12::DescriptorType::CBV, 0, CG::DX12::ShaderVisibility::Pixel);
//    rsDesc.AddDescriptorTable(CG::DX12::ShaderVisibility::Pixel);
//    rsDesc.AddDescriptorRange(CG::DX12::RangeType::SRV, 0, 1);
//    rsDesc.AddDescriptor(CG::DX12::DescriptorType::CBV, 1, CG::DX12::ShaderVisibility::Pixel);
//    rsDesc.AddStaticSampler(0, CG::DX12::ShaderVisibility::Pixel);
//    rsDesc.AddFlag(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
//    CG::DX12::RootSignature rs;
//    rs.Initialize(device, rsDesc);
//
//    CG::DX12::ShaderCompiler shaderCompiler;
//    shaderCompiler.Initialize();
//    auto vs = shaderCompiler.Compile("Object3d.VS.hlsl", L"vs_6_0");
//    auto ps = shaderCompiler.Compile("Object3d.PS.hlsl", L"ps_6_0");
//
//    CG::DX12::GraphicsPipelineStateDesc gps;
//    gps.SetRootSignature(rs);
//    gps.SetVertexShader(vs);
//    gps.SetPixelShader(ps);
//    gps.AddInputElementVertex("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0);
//    gps.AddInputElementVertex("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0);
//    gps.AddInputElementVertex("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0);
//    gps.SetPrimitiveTopologyType(CG::DX12::PrimitiveTopology::Triangle);
//    gps.SetRasterizerState(CG::DX12::CullMode::Back, CG::DX12::FillMode::Solid);
//    gps.AddRenderTargetState(CG::DX12::BlendMode::Normal, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
//    gps.SetSampleState();
//
//
//    CG::DX12::PipelineState pso;
//    pso.Initialize(device, gps);
//
//    {
//        CG::Window window;
//        window.Initialize("Game", 1280, 720);
//
//        CG::GraphicsEngine graphicsEngine;
//        graphicsEngine.Initialize(&window);
//
//        CG::GUI gui;
//        gui.Initialize(window, graphicsEngine);
//
//        while (window.ProcessMessage()) {
//            gui.NewFrame();
//
//
//        }
//
//        gui.Finalize();
//        window.Finalize();
//    }
//
//    //    Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource{ nullptr };
//    //    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap{ nullptr };
//    //    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
//    //    {
//    //        depthStencilResource = CreateDepthStencilTextureResource(device, int32_t(kClientWidth), int32_t(kClientHeight));
//    //        dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
//    //        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
//    //        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//    //        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
//    //        dsvHandle = GetCPUDescriptorHandle(dsvDescriptorHeap, descriptorSizeDSV, 0);
//    //        device->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvHandle);
//    //    }
//    //
//    //    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap{ nullptr };
//    //    {
//    //        srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
//    //    }
//    //    // ImGuiの初期化
//    //    {
//    //        IMGUI_CHECKVERSION();
//    //        ImGui::CreateContext();
//    //        ImGui::StyleColorsDark();
//    //        ImGui_ImplWin32_Init(hwnd);
//    //        ImGui_ImplDX12_Init(
//    //            device.Get(),
//    //            swapChainDesc.BufferCount,
//    //            rtvDesc.Format,
//    //            srvDescriptorHeap.Get(),
//    //            GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 0),
//    //            GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 0));
//    //    }
//    //
//    //    ShaderCompiler shaderCompiler;
//    //    shaderCompiler.Initalize();
//    //
//    //    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature{ nullptr };
//    //    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState{ nullptr };
//    //    {
//    //        HRESULT hr = S_FALSE;
//    //
//    //        D3D12_ROOT_PARAMETER rootParameters[4] = {};
//    //        rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//    //        rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
//    //        rootParameters[0].Descriptor.ShaderRegister = 0;
//    //
//    //        rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//    //        rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//    //        rootParameters[1].Descriptor.ShaderRegister = 0;
//    //
//    //        D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
//    //        descriptorRange[0].BaseShaderRegister = 0;
//    //        descriptorRange[0].NumDescriptors = 1;
//    //        descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//    //        descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//    //
//    //        rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//    //        rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//    //        rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
//    //        rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
//    //
//    //        rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
//    //        rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//    //        rootParameters[3].Descriptor.ShaderRegister = 1;
//    //
//    //        D3D12_STATIC_SAMPLER_DESC staticSampler[1] = {};
//    //        staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
//    //        staticSampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//    //        staticSampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//    //        staticSampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//    //        staticSampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
//    //        staticSampler[0].MaxLOD = D3D12_FLOAT32_MAX;
//    //        staticSampler[0].ShaderRegister = 0;
//    //        staticSampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//    //
//    //        D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
//    //        descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//    //        descriptionRootSignature.pParameters = rootParameters;
//    //        descriptionRootSignature.NumParameters = _countof(rootParameters);
//    //        descriptionRootSignature.pStaticSamplers = staticSampler;
//    //        descriptionRootSignature.NumStaticSamplers = _countof(staticSampler);
//    //
//    //        Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
//    //        Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
//    //
//    //        hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
//    //        if (FAILED(hr)) {
//    //            Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
//    //            assert(false);
//    //        }
//    //        hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
//    //        assert(SUCCEEDED(hr));
//    //
//    //
//    //        // インプットレイアウト
//    //        D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
//    //
//    //        inputElementDescs[0].SemanticName = "POSITION";
//    //        inputElementDescs[0].SemanticIndex = 0;
//    //        inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//    //        inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
//    //
//    //        inputElementDescs[1].SemanticName = "TEXCOORD";
//    //        inputElementDescs[1].SemanticIndex = 0;
//    //        inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
//    //        inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
//    //
//    //        inputElementDescs[2].SemanticName = "NORMAL";
//    //        inputElementDescs[2].SemanticIndex = 0;
//    //        inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
//    //        inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
//    //
//    //        D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
//    //        inputLayoutDesc.pInputElementDescs = inputElementDescs;
//    //        inputLayoutDesc.NumElements = _countof(inputElementDescs);
//    //
//    //        // ブレンドステート
//    //        D3D12_BLEND_DESC blendDesc{};
//    //        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//    //
//    //        D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
//    //        depthStencilDesc.DepthEnable = true;
//    //        depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//    //        depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//    //
//    //        // ラスタライザステート
//    //        D3D12_RASTERIZER_DESC rasterizerDesc{};
//    //        rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
//    //        rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
//    //
//    //        // シェーダーをコンパイル
//    //        Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = shaderCompiler.Compile(L"Object3d.VS.hlsl", L"vs_6_0");
//    //        assert(vertexShaderBlob != nullptr);
//    //        Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = shaderCompiler.Compile(L"Object3d.PS.hlsl", L"ps_6_0");
//    //        assert(pixelShaderBlob != nullptr);
//    //
//    //        // パイプライン生成
//    //        D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
//    //        graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
//    //        graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
//    //        graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };
//    //        graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize() };
//    //        graphicsPipelineStateDesc.BlendState = blendDesc;
//    //        graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
//    //        graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
//    //        graphicsPipelineStateDesc.NumRenderTargets = 1;
//    //        graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
//    //        graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
//    //        graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//    //        graphicsPipelineStateDesc.SampleDesc.Count = 1;
//    //        graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
//    //
//    //        hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(pipelineState.GetAddressOf()));
//    //        assert(SUCCEEDED(hr));
//    //    }
//    //
//    //    Transform cameraTransform{ {1.0f,1.0f,1.0f}, {} ,{0.0f,0.0f,-10.0f} };
//    //    ConstantResource directionalLightResource;
//    //    DirectionalLightConstantData directionalLightConstantData{};
//    //    {
//    //        directionalLightConstantData.color = { 1.0f,1.0f,1.0f,1.0f };
//    //        directionalLightConstantData.direction = { 0.0f,-1.0f,0.0f };
//    //        directionalLightConstantData.intensity = 1.0f;
//    //        directionalLightResource.resource = CreateBufferResource(device, sizeof(directionalLightConstantData));
//    //        directionalLightResource.resource->Map(0, nullptr, &directionalLightResource.mappedData);
//    //        memcpy(directionalLightResource.mappedData, &directionalLightConstantData, sizeof(directionalLightConstantData));
//    //    }
//    //
//    //    Transform transformSphere{ {1.0f,1.0f,1.0f}, {}, {} };
//    //    Vector4 colorSphere{ 1.0f,1.0f,1.0f,1.0f };
//    //    Transform2D uvTransformSphere{ {1.0f,1.0f}, 0.0f ,{} };
//    //    VertexResource vertexResourceSphere;
//    //    IndexResource indexResourceSphere;
//    //    ConstantResource materialResourceSphere;
//    //    ConstantResource transformationMatrixResourceSphere;
//    //    {
//    //        // 球を作成
//    //        std::vector<VertexData> vertices;
//    //        std::vector<uint32_t> indices;
//    //        {
//    //            const int32_t kSubdivision = 16;
//    //
//    //            const float kLonEvery = Math::TwoPi / float(kSubdivision);
//    //            const float kLatEvery = Math::Pi / float(kSubdivision);
//    //            const size_t kLatVertexCount = size_t(kSubdivision + 1);
//    //            const size_t kLonVertexCount = size_t(kSubdivision + 1);
//    //            const size_t kVertexCount = kLonVertexCount * kLatVertexCount;
//    //
//    //            vertices.resize(kVertexCount);
//    //
//    //            auto CalcPosition = [](float lat, float lon) {
//    //                return Vector4{
//    //                    .x{ std::cos(lat) * std::cos(lon) },
//    //                    .y{ std::sin(lat) },
//    //                    .z{ std::cos(lat) * std::sin(lon) },
//    //                    .w{ 1.0f }
//    //                };
//    //            };
//    //            auto CalcTexcoord = [](size_t latIndex, size_t lonIndex) {
//    //                return Vector2{
//    //                    .x{ float(lonIndex) / float(kSubdivision) },
//    //                    .y{ 1.0f - float(latIndex) / float(kSubdivision) },
//    //                };
//    //            };
//    //
//    //
//    //            for (size_t latIndex = 0; latIndex < kLonVertexCount; ++latIndex) {
//    //                float lat = -Math::HalfPi + kLatEvery * latIndex;
//    //
//    //                for (size_t lonIndex = 0; lonIndex < kLatVertexCount; ++lonIndex) {
//    //                    float lon = lonIndex * kLonEvery;
//    //
//    //                    size_t vertexIndex = latIndex * kLatVertexCount + lonIndex;
//    //                    vertices[vertexIndex].position = CalcPosition(lat, lon);
//    //                    vertices[vertexIndex].texcoord = CalcTexcoord(latIndex, lonIndex);
//    //                    vertices[vertexIndex].normal = static_cast<Vector3>(vertices[vertexIndex].position);
//    //                }
//    //            }
//    //
//    //            const size_t kIndexCount = size_t(kSubdivision * kSubdivision) * 6;
//    //            indices.resize(kIndexCount);
//    //
//    //            for (uint32_t i = 0; i < kSubdivision; ++i) {
//    //                uint32_t y0 = i * kLatVertexCount;
//    //                uint32_t y1 = (i + 1) * uint32_t(kLatVertexCount);
//    //
//    //                for (uint32_t j = 0; j < kSubdivision; ++j) {
//    //                    uint32_t index0 = y0 + j;
//    //                    uint32_t index1 = y1 + j;
//    //                    uint32_t index2 = y0 + j + 1;
//    //                    uint32_t index3 = y1 + j + 1;
//    //
//    //                    uint32_t indexIndex = (i * kSubdivision + j) * 6;
//    //                    indices[indexIndex++] = index0;
//    //                    indices[indexIndex++] = index1;
//    //                    indices[indexIndex++] = index2;
//    //                    indices[indexIndex++] = index1;
//    //                    indices[indexIndex++] = index3;
//    //                    indices[indexIndex++] = index2;
//    //                }
//    //            }
//    //        }
//    //
//    //        ID3D12GraphicsCommandList* c;
//    //        c->DrawIndexedInstanced()
//    //
//    //        vertexResourceSphere.resource = CreateBufferResource(device, sizeof(vertices[0]) * vertices.size());
//    //        vertexResourceSphere.strideSize = sizeof(vertices[0]);
//    //        VertexData* vertexData = nullptr;
//    //        vertexResourceSphere.resource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
//    //        memcpy(vertexData, vertices.data(), sizeof(vertices[0]) * vertices.size());
//    //        vertexResourceSphere.resource->Unmap(0, nullptr);
//    //
//    //        indexResourceSphere.resource = CreateBufferResource(device, sizeof(indices[0]) * indices.size());
//    //        uint32_t* indexData = nullptr;
//    //        indexResourceSphere.resource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
//    //        memcpy(indexData, indices.data(), sizeof(indices[0]) * indices.size());
//    //        indexResourceSphere.resource->Unmap(0, nullptr);
//    //
//    //        MaterialConstantData materialData{};
//    //        materialData.color = colorSphere;
//    //        materialData.enableLighting = true;
//    //        materialData.uvTransform = MakeAffineMatrix(ToVector3(uvTransformSphere.scale, 1.0f), { 0.0f,0.0f,uvTransformSphere.rotate }, ToVector3(uvTransformSphere.translate, 0.0f));
//    //        materialResourceSphere.resource = CreateBufferResource(device, sizeof(materialData));
//    //        materialResourceSphere.resource->Map(0, nullptr, &materialResourceSphere.mappedData);
//    //        memcpy(materialResourceSphere.mappedData, &materialData, sizeof(materialData));
//    //
//    //        Matrix4x4 worldMatrix = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
//    //        Matrix4x4 viewMatrix = Inverse(MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate));
//    //        Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
//    //        TransformationConstantData transformationData{ .wvp{worldMatrix * viewMatrix * projectionMatrix}, .world{worldMatrix} };
//    //        transformationMatrixResourceSphere.resource = CreateBufferResource(device, sizeof(transformationData));
//    //        transformationMatrixResourceSphere.resource->Map(0, nullptr, &transformationMatrixResourceSphere.mappedData);
//    //        memcpy(transformationMatrixResourceSphere.mappedData, &transformationData, sizeof(transformationData));
//    //    }
//    //
//    //    Transform transformSprite{ {1.0f,1.0f,1.0f}, {}, {} };
//    //    Vector4 colorSprite{ 1.0f,1.0f,1.0f,1.0f };
//    //    Transform2D uvTransformSprite{ {1.0f,1.0f}, 0.0f ,{} };
//    //    VertexResource vertexResourceSprite;
//    //    IndexResource indexResourceSprite;
//    //    ConstantResource transformationMatrixResourceSprite;
//    //    ConstantResource materialResourceSprite;
//    //    {
//    //        VertexData vertices[]{
//    //            {.position{   0.0f, 360.0f, 0.0f, 1.0f }, .texcoord{ 0.0f, 1.0f }, .normal{ 0.0f, 0.0f, -1.0f } },
//    //            {.position{   0.0f,   0.0f, 0.0f, 1.0f }, .texcoord{ 0.0f, 0.0f }, .normal{ 0.0f, 0.0f, -1.0f } },
//    //            {.position{ 640.0f, 360.0f, 0.0f, 1.0f }, .texcoord{ 1.0f, 1.0f }, .normal{ 0.0f, 0.0f, -1.0f } },
//    //            {.position{ 640.0f,   0.0f, 0.0f, 1.0f }, .texcoord{ 1.0f, 0.0f }, .normal{ 0.0f, 0.0f, -1.0f } },
//    //        };
//    //        vertexResourceSprite.resource = CreateBufferResource(device, sizeof(vertices));
//    //        vertexResourceSprite.strideSize = sizeof(vertices[0]);
//    //
//    //        VertexData* vertexData = nullptr;
//    //        vertexResourceSprite.resource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
//    //        memcpy(vertexData, vertices, sizeof(vertices));
//    //        vertexResourceSprite.resource->Unmap(0, nullptr);
//    //
//    //        uint32_t indices[]{
//    //            0, 1, 2,
//    //            1, 3, 2
//    //        };
//    //        indexResourceSprite.resource = CreateBufferResource(device, sizeof(indices));
//    //
//    //        uint32_t* indexData = nullptr;
//    //        indexResourceSprite.resource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
//    //        memcpy(indexData, indices, sizeof(indices));
//    //        indexResourceSprite.resource->Unmap(0, nullptr);
//    //
//    //        MaterialConstantData materialData{};
//    //        materialData.color = colorSprite;
//    //        materialData.enableLighting = false;
//    //        materialData.uvTransform = MakeAffineMatrix(ToVector3(uvTransformSprite.scale, 1.0f), { 0.0f,0.0f,uvTransformSprite.rotate }, ToVector3(uvTransformSprite.translate, 0.0f));
//    //        materialResourceSprite.resource = CreateBufferResource(device, sizeof(materialData));
//    //        materialResourceSprite.resource->Map(0, nullptr, &materialResourceSprite.mappedData);
//    //        memcpy(materialResourceSprite.mappedData, &materialData, sizeof(materialData));
//    //
//    //        TransformationConstantData transformationConstantData{ .wvp{ MakeIdentityMatrix() }, .world{ MakeIdentityMatrix() } };
//    //        transformationMatrixResourceSprite.resource = CreateBufferResource(device, sizeof(transformationConstantData));
//    //        transformationMatrixResourceSprite.resource->Map(0, nullptr, &transformationMatrixResourceSprite.mappedData);
//    //        memcpy(transformationMatrixResourceSprite.mappedData, &transformationConstantData, sizeof(transformationConstantData));
//    //    }
//    //
//    //    TextureResource textureResource{ nullptr };
//    //    {
//    //        auto mipImages = LoadTexture("resources/uvChecker.png");
//    //        const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
//    //        textureResource.resource = CreateTextureResource(device, metadata);
//    //
//    //        Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource.resource, mipImages, device, commandList->Get());
//    //        commandList->ExcuteCommand();
//    //        commandList->WaitForGPU();
//    //        commandList->Reset();
//    //
//    //        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//    //        srvDesc.Format = metadata.format;
//    //        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//    //        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//    //        srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
//    //
//    //        textureResource.cpuHandle = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
//    //        textureResource.gpuHandle = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 1);
//    //
//    //        device->CreateShaderResourceView(textureResource.resource.Get(), &srvDesc, textureResource.cpuHandle);
//    //    }
//    //
//    //    TextureResource textureResource2{ nullptr };
//    //    {
//    //        auto mipImages = LoadTexture("resources/monsterBall.png");
//    //        const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
//    //        textureResource2.resource = CreateTextureResource(device, metadata);
//    //
//    //        Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureData(textureResource2.resource, mipImages, device, commandList->Get());
//    //        commandList->ExcuteCommand();
//    //        commandList->WaitForGPU();
//    //        commandList->Reset();
//    //
//    //        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//    //        srvDesc.Format = metadata.format;
//    //        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//    //        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//    //        srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
//    //
//    //        textureResource2.cpuHandle = GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
//    //        textureResource2.gpuHandle = GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 2);
//    //
//    //        device->CreateShaderResourceView(textureResource2.resource.Get(), &srvDesc, textureResource2.cpuHandle);
//    //    }
//    //
//    //    bool showObject = true;
//    //    bool showSprite = true;
//    //    bool useMonsterBall = false;
//
//    commandList.Reset();
//
//    {
//        MSG msg{};
//        // ウィンドウの×ボタンがが押されるまでループ
//        while (msg.message != WM_QUIT) {
//            // Windowにメッセージが来てたら最優先で処理させる
//            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//                TranslateMessage(&msg);
//                DispatchMessage(&msg);
//            }
//            else {
//                auto cmdList = commandList.GetCommandList();
//
//                auto barrier = swapChain.GetCurrentResource().TransitionBarrier(CG::DX12::Resource::State::RenderTarget);
//                cmdList->ResourceBarrier(1, &barrier);
//                auto rtvHandle = swapChain.GetCurrentRenderTargetView().GetDescriptor().GetCPUHandle();
//                cmdList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
//                const float cc[4] = { 0.0f,0.0f,0.0f,1.0f };
//                cmdList->ClearRenderTargetView(rtvHandle, cc, 0, nullptr);
//
//
//                barrier = swapChain.GetCurrentResource().TransitionBarrier(CG::DX12::Resource::State::Present);
//                cmdList->ResourceBarrier(1, &barrier);
//
//
//
//                commandList.Close();
//                //commandQueue.ExcuteCommandList(commandList);
//                commandQueue.ExcuteCommandLists({ &commandList });
//                swapChain.Present(1);
//                fence.Signal(commandQueue);
//                fence.Wait();
//                commandList.Reset();
//
//
//                //ImGui_ImplWin32_NewFrame();
//                //ImGui_ImplDX12_NewFrame();
//                //ImGui::NewFrame();
//
//                //auto cmdList = commandList->Get();
//
//                //// これから書き込むバックバッファインデックスを取得
//                //uint32_t backBufferIndex = swapChain->GetCurrentBackBufferIndex();
//                //// レンダ―ターゲットに遷移
//                //auto barrier = swapChainResource[backBufferIndex].TransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);
//                //// TransitionBarrierを張る
//                //cmdList->ResourceBarrier(1, &barrier);
//
//                //// 描画先のRTVを設定
//                //cmdList->OMSetRenderTargets(1, &rtvHandle[backBufferIndex], false, &dsvHandle);
//                //// 指定した色で画面全体をクリア
//                //float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
//                //cmdList->ClearRenderTargetView(rtvHandle[backBufferIndex], clearColor, 0, nullptr);
//                //cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
//
//                //D3D12_VIEWPORT viewport{};
//                //viewport.Width = static_cast<float>(kClientWidth);
//                //viewport.Height = static_cast<float>(kClientHeight);
//                //viewport.TopLeftX = 0.0f;
//                //viewport.TopLeftY = 0.0f;
//                //viewport.MinDepth = 0.0f;
//                //viewport.MaxDepth = 1.0f;
//                //cmdList->RSSetViewports(1, &viewport);
//
//                //D3D12_RECT scissorRect{};
//                //scissorRect.left = 0;
//                //scissorRect.right = kClientWidth;
//                //scissorRect.top = 0;
//                //scissorRect.bottom = kClientHeight;
//                //cmdList->RSSetScissorRects(1, &scissorRect);
//
//                //ID3D12DescriptorHeap* ppHeaps[] = { srvDescriptorHeap.Get() };
//                //cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
//
//                ////-----------------------------------------------------------------------------------------//
//                //ImGui::SetNextWindowPos({ kClientWidth - 300, 0.0f }, ImGuiCond_Once);
//                //ImGui::SetNextWindowSize({ 300, 200.0f }, ImGuiCond_Once);
//                //ImGui::Begin("Window");
//                //if (ImGui::Checkbox("Show objects?", &showObject)) {
//
//                //}
//                //ImGui::Checkbox("Show sprites?", &showSprite);
//                //ImGui::Checkbox("Use MonsterBall?", &useMonsterBall);
//                //if (ImGui::TreeNode("Camera")) {
//                //    ImGui::DragFloat3("Position", &cameraTransform.translate.x, 0.01f);
//                //    Vector3 deg = cameraTransform.rotate * Math::ToDegree;
//                //    ImGui::DragFloat3("Rotate", &deg.x, 0.1f, -360.0f, 360.0f);
//                //    cameraTransform.rotate = deg * Math::ToRadian;
//                //    ImGui::TreePop();
//                //}
//                //if (ImGui::TreeNode("DirectionalLight")) {
//                //    ImGui::ColorEdit4("Color", &directionalLightConstantData.color.x);
//                //    ImGui::DragFloat3("Direction", &directionalLightConstantData.direction.x, 0.01f);
//                //    directionalLightConstantData.direction = Normalize(directionalLightConstantData.direction);
//                //    ImGui::DragFloat("Intensity", &directionalLightConstantData.intensity, 0.01f);
//                //    memcpy(directionalLightResource.mappedData, &directionalLightConstantData, sizeof(directionalLightConstantData));
//                //    ImGui::TreePop();
//                //}
//                //ImGui::End();
//
//                //if (showObject) {
//                //    ImGui::Begin("Object");
//                //    if (ImGui::TreeNode("Transform")) {
//                //        ImGui::DragFloat3("Translate", &transformSphere.translate.x, 0.01f);
//                //        Vector3 deg = transformSphere.rotate * Math::ToDegree;
//                //        ImGui::DragFloat3("Rotate", &deg.x, 0.1f, -360.0f, 360.0f);
//                //        transformSphere.rotate = deg * Math::ToRadian;
//                //        ImGui::DragFloat3("Scale", &transformSphere.scale.x, 0.01f);
//                //        ImGui::TreePop();
//                //    }
//                //    if (ImGui::TreeNode("Material")) {
//                //        ImGui::ColorEdit4("Color", &colorSphere.x);
//                //        if (ImGui::TreeNode("UVTransform")) {
//                //            ImGui::DragFloat2("Translate", &uvTransformSphere.translate.x, 0.01f);
//                //            float deg = uvTransformSphere.rotate * Math::ToDegree;
//                //            ImGui::DragFloat("Rotate", &deg, 0.1f, -360.0f, 360.0f);
//                //            uvTransformSphere.rotate = deg * Math::ToRadian;
//                //            ImGui::DragFloat2("Scale", &uvTransformSphere.scale.x, 0.01f);
//                //            ImGui::TreePop();
//                //        }
//                //        ImGui::TreePop();
//                //    }
//                //    ImGui::End();
//                //}
//
//                //if (showSprite) {
//                //    ImGui::Begin("Sprite");
//                //    if (ImGui::TreeNode("Transform")) {
//                //        ImGui::DragFloat3("Translate", &transformSprite.translate.x, 0.01f);
//                //        Vector3 deg = transformSprite.rotate * Math::ToDegree;
//                //        ImGui::DragFloat3("Rotate", &deg.x, 0.1f, -360.0f, 360.0f);
//                //        transformSprite.rotate = deg * Math::ToRadian;
//                //        ImGui::DragFloat3("Scale", &transformSprite.scale.x, 0.01f);
//                //        ImGui::TreePop();
//                //    }
//                //    if (ImGui::TreeNode("Material")) {
//                //        ImGui::ColorEdit4("Color", &colorSprite.x);
//                //        if (ImGui::TreeNode("UVTransform")) {
//                //            ImGui::DragFloat2("Translate", &uvTransformSprite.translate.x, 0.01f);
//                //            float deg = uvTransformSprite.rotate * Math::ToDegree;
//                //            ImGui::DragFloat("Rotate", &deg, 0.1f, -360.0f, 360.0f);
//                //            uvTransformSprite.rotate = deg * Math::ToRadian;
//                //            ImGui::DragFloat2("Scale", &uvTransformSprite.scale.x, 0.01f);
//                //            ImGui::TreePop();
//                //        }
//                //        ImGui::TreePop();
//                //    }
//
//                //    ImGui::End();
//                //}
//
//
//                //cmdList->SetGraphicsRootSignature(rootSignature.Get());
//                //cmdList->SetPipelineState(pipelineState.Get());
//                //cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//                //cmdList->SetGraphicsRootConstantBufferView(3, directionalLightResource.resource->GetGPUVirtualAddress());
//                //if (showObject) {
//                //    transformSphere.rotate.y += 0.01f;
//                //    Matrix4x4 worldMatrix = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
//                //    Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
//                //    Matrix4x4 viewMatrix = Inverse(cameraMatrix);
//                //    Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
//                //    Matrix4x4 wvp = worldMatrix * viewMatrix * projectionMatrix;
//                //    TransformationConstantData transformationConstantData{ .wvp{wvp}, .world{worldMatrix} };
//                //    memcpy(transformationMatrixResourceSphere.mappedData, &transformationConstantData, sizeof(transformationConstantData));
//
//                //    MaterialConstantData materialData{};
//                //    materialData.color = colorSphere;
//                //    materialData.enableLighting = true;
//                //    materialData.uvTransform = MakeAffineMatrix(ToVector3(uvTransformSphere.scale, 1.0f), { 0.0f,0.0f,uvTransformSphere.rotate }, ToVector3(uvTransformSphere.translate, 0.0f));
//                //    memcpy(materialResourceSphere.mappedData, &materialData, sizeof(materialData));
//
//                //    auto vertexBufferView = vertexResourceSphere.View();
//                //    cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
//                //    auto indexBufferView = indexResourceSphere.View();
//                //    cmdList->IASetIndexBuffer(&indexBufferView);
//                //    cmdList->SetGraphicsRootConstantBufferView(0, transformationMatrixResourceSphere.resource->GetGPUVirtualAddress());
//                //    cmdList->SetGraphicsRootConstantBufferView(1, materialResourceSphere.resource->GetGPUVirtualAddress());
//                //    cmdList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureResource2.gpuHandle : textureResource.gpuHandle);
//                //    uint32_t indexCount = indexBufferView.SizeInBytes / sizeof(uint32_t);
//                //    cmdList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
//                //}
//                //if (showSprite) {
//                //    Matrix4x4 worldMatrix = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
//                //    Matrix4x4 viewMatrix = MakeIdentityMatrix();
//                //    Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);
//                //    Matrix4x4 wvp = worldMatrix * viewMatrix * projectionMatrix;
//                //    TransformationConstantData transformationConstantData{ .wvp{wvp}, .world{worldMatrix} };
//                //    memcpy(transformationMatrixResourceSprite.mappedData, &transformationConstantData, sizeof(transformationConstantData));
//
//                //    MaterialConstantData materialData{};
//                //    materialData.color = colorSprite;
//                //    materialData.enableLighting = false;
//                //    materialData.uvTransform = MakeAffineMatrix(ToVector3(uvTransformSprite.scale, 1.0f), { 0.0f,0.0f,uvTransformSprite.rotate }, ToVector3(uvTransformSprite.translate, 0.0f));
//                //    memcpy(materialResourceSprite.mappedData, &materialData, sizeof(materialData));
//
//                //    auto vertexBufferView = vertexResourceSprite.View();
//                //    cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
//                //    auto indexBufferView = indexResourceSprite.View();
//                //    cmdList->IASetIndexBuffer(&indexBufferView);
//                //    cmdList->SetGraphicsRootConstantBufferView(0, transformationMatrixResourceSprite.resource->GetGPUVirtualAddress());
//                //    cmdList->SetGraphicsRootConstantBufferView(1, materialResourceSprite.resource->GetGPUVirtualAddress());
//                //    cmdList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureResource2.gpuHandle : textureResource.gpuHandle);
//                //    uint32_t indexCount = indexBufferView.SizeInBytes / sizeof(uint32_t);
//                //    cmdList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
//                //}
//
//
//                ////-----------------------------------------------------------------------------------------//
//
//
//                //ImGui::Render();
//                //ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());
//
//                //// RenderTargetからPresentへ
//                //barrier = swapChainResource[backBufferIndex].TransitionBarrier(D3D12_RESOURCE_STATE_PRESENT);
//                //// TransitionBarrierを張る
//                //cmdList->ResourceBarrier(1, &barrier);
//
//                //commandList->ExcuteCommand();
//                //// GPUとOSに画面交換を行うよう通知する
//                //swapChain->Present(1, 0);
//                //commandList->WaitForGPU();
//                //commandList->Reset();
//            }
//        }
//    }
//
//    {
//        /* ImGui_ImplDX12_Shutdown();
//         ImGui_ImplWin32_Shutdown();
//         ImGui::DestroyContext();*/
//
//         /* indexResourceSprite.resource->Release();
//          transformationMatrixResourceSprite.resource->Release();
//          materialResourceSprite.resource->Release();
//          vertexResourceSprite.resource->Release();
//          textureResource2.resource->Release();
//          textureResource.resource->Release();
//          transformationMatrixResourceSphere.resource->Release();
//          materialResourceSphere.resource->Release();
//          indexResourceSphere.resource->Release();
//          vertexResourceSphere.resource->Release();
//          directionalLightResource.resource->Release();
//          pipelineState->Release();
//          rootSignature->Release();
//          srvDescriptorHeap->Release();
//          dsvDescriptorHeap->Release();
//          depthStencilResource->Release();
//          swapChainResource[0].resource->Release();
//          swapChainResource[1].resource->Release();
//          rtvDescriptorHeap->Release();
//          swapChain->Release();
//          commandList.reset();
//          commandQueue->Release();
//          device->Release();
//          dxgiFactory->Release();*/
//        CloseWindow(hwnd);
//
//
//    }
//
//    CoUninitialize();
//
//    return 0;
//}

//D3D12_RESOURCE_BARRIER GPUResource::TransitionBarrier(D3D12_RESOURCE_STATES nextState) {
//    assert(resource);
//    D3D12_RESOURCE_BARRIER barrier{};
//    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//    barrier.Transition.pResource = resource.Get();
//    barrier.Transition.StateBefore = currentState;
//    barrier.Transition.StateAfter = nextState;
//    currentState = nextState;
//    return barrier;
//}
//
//D3D12_VERTEX_BUFFER_VIEW VertexResource::View() {
//    assert(resource);
//    D3D12_VERTEX_BUFFER_VIEW view{};
//    view.BufferLocation = resource->GetGPUVirtualAddress();
//    view.SizeInBytes = static_cast<uint32_t>(resource->GetDesc().Width);
//    view.StrideInBytes = strideSize;
//    return view;
//}
//
//D3D12_INDEX_BUFFER_VIEW IndexResource::View() {
//    assert(resource);
//    D3D12_INDEX_BUFFER_VIEW view{};
//    view.BufferLocation = resource->GetGPUVirtualAddress();
//    view.SizeInBytes = static_cast<uint32_t>(resource->GetDesc().Width);
//    view.Format = DXGI_FORMAT_R32_UINT;
//    return view;
//}

#include "Scene.h"
#include "GameObject.h"
#include "Component.h"
#include "ModelRenderer.h"
#include "Camera.h"


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    CG::DX12::D3DResourceLeakChecker leakChecker;
    {
        CG::Window window;
        window.Initialize("Game", 1280, 720);

        CG::GraphicsEngine graphicsEngine;
        graphicsEngine.Initialize(&window);

        CG::ImGuiManager imguiManager;
        imguiManager.Initialize(window, graphicsEngine);

        CG::ResourceManager resourceManager;
        resourceManager.Initialize(&graphicsEngine);
        auto model = resourceManager.LoadModelFromObj("Resources", "axis.obj");
        auto multiMesh = resourceManager.LoadModelFromObj("Resources", "multiMesh.obj");
        auto multiMaterial = resourceManager.LoadModelFromObj("Resources", "multiMaterial.obj");

        CG::Scene scene;
        {
            auto mainCamera = scene.AddGameObject();
            auto camera = mainCamera->AddComponent<CG::Camera>();
            CG::Camera::SetMainCamera(camera);
        }

        {
            auto object = scene.AddGameObject();
            auto modelRenderer = object->AddComponent<CG::ModelRenderer>();
            modelRenderer->SetModel();
        }

        window.Show();

        while (window.ProcessMessage()) {
            imguiManager.NewFrame();
            auto& commandList = graphicsEngine.PreDraw();

            ImGui::Begin("Setting");
            ImGui::End();


            imguiManager.Render(commandList);
            graphicsEngine.PostDraw(commandList);
        }

        imguiManager.Finalize();
        window.Finalize();
    }

}