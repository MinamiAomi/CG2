#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <cassert>
#include "StringUtils.h"
#include "MathUtils.h"
#include "StringUtils.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")


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

struct ConstantBuffer {
	ID3D12Resource* resource = nullptr;
	void* mapData = nullptr;
	size_t sizeInByte = {};
};

// ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	// メッセージに対してゲーム固有の処理を行う
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
void CreateGameWindow(const std::string& windowName);
void InitalizeDirectX();
void CreateDirectXCommand();
void CreateScreenRenderTarget();
void InitalizeDXC();
void CreatePipelineState();
void CreateTriangle();

void MainLoop();

void Release();

IDxcBlob* CompilerShader(const std::wstring& filePath, const wchar_t* profile);
ID3D12Resource* CreateBufferResource(size_t sizeInBytes);

// グローバル変数
HWND g_hwnd = nullptr;
IDXGIFactory7* g_dxgiFactory = nullptr;
ID3D12Device* g_device = nullptr;
ID3D12CommandQueue* g_commandQueue = nullptr;
ID3D12CommandAllocator* g_commandAllocator = nullptr;
ID3D12GraphicsCommandList* g_commandList = nullptr;
ID3D12Fence* g_fence = nullptr;
uint64_t g_fenceValue = 0;
HANDLE g_fenceEvent = nullptr;
IDXGISwapChain4* g_swapChain = nullptr;
ID3D12DescriptorHeap* g_rtvDescriptorHeap = nullptr;
ID3D12Resource* g_swapChainResource[kSwapChainBufferCount] = { nullptr };
D3D12_CPU_DESCRIPTOR_HANDLE g_rtvHandle[kSwapChainBufferCount] = {};

IDxcUtils* g_dxcUtils = nullptr;
IDxcCompiler3* g_dxcCompiler = nullptr;
IDxcIncludeHandler* g_includeHandler = nullptr;

ID3D12RootSignature* g_rootSignature = nullptr;
ID3D12PipelineState* g_pipelineState = nullptr;
ID3D12Resource* g_vertexResource = nullptr;
D3D12_VERTEX_BUFFER_VIEW g_vertexBufferView{};

ConstantBuffer g_materialBuffer;
Transform g_transform{ {1.0f,1.0f,1.0f}, {}, {} };
ConstantBuffer g_wvpBuffer;

Transform g_cameraTransform{ {1.0f,1.0f,1.0f}, {} ,{0.0f,0.0f,-5.0f} };

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// 出力ウィンドウに文字出力
	Log("Hello,DirectX!\n");

	CreateGameWindow("CG2");
	InitalizeDirectX();
	CreateDirectXCommand();
	CreateScreenRenderTarget();
	InitalizeDXC();
	CreatePipelineState();
	CreateTriangle();

	MainLoop();

	Release();

	return 0;
}

void CreateGameWindow(const std::string& windowName) {
	// ウィンドウクラスを生成
	WNDCLASS wc{};
	// ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;
	// ウィンドウクラス名
	wc.lpszClassName = L"CG2WindowClass";
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウィンドウクラスを登録
	RegisterClass(&wc);

	// ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc{ 0,0,static_cast<LONG>(kClientWidth),static_cast<LONG>(kClientHeight) };
	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	auto wname = ConvertString(windowName);

	// ウィンドウの生成
	g_hwnd = CreateWindow(
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

	ShowWindow(g_hwnd, SW_SHOW);
}

void InitalizeDirectX() {
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
	hr = CreateDXGIFactory(IID_PPV_ARGS(&g_dxgiFactory));
	assert(SUCCEEDED(hr));

	// 使用するアダプタ用の変数
	IDXGIAdapter4* useAdapter = nullptr;

	// 良い順にアダプターを頼む
	for (uint32_t i = 0;
		g_dxgiFactory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) != DXGI_ERROR_NOT_FOUND;
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
		hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&g_device));
		// 指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	assert(g_device != nullptr);
	useAdapter->Release();
	Log("Complete create D3D12Device!!!\n"); // 初期化完了のログを出す

#ifdef _DEBUG
	// デバッグ時のみ
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(g_device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
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

void CreateDirectXCommand() {
	HRESULT hr = S_FALSE;
	// コマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = g_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&g_commandQueue));
	assert(SUCCEEDED(hr));

	// コマンドアロケータを生成
	hr = g_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocator));
	assert(SUCCEEDED(hr));

	// コマンドリストを生成
	hr = g_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocator, nullptr, IID_PPV_ARGS(&g_commandList));
	assert(SUCCEEDED(hr));

	// フェンスを生成
	hr = g_device->CreateFence(g_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence));
	assert(SUCCEEDED(hr));

	g_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(g_fenceEvent != nullptr);
}

void CreateScreenRenderTarget() {
	HRESULT hr = S_FALSE;
	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = kClientWidth;		// 画面幅
	swapChainDesc.Height = kClientHeight;	// 画面高
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 色の形式
	swapChainDesc.SampleDesc.Count = 1;					// マルチサンプル市内
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 描画ターゲットとして利用する
	swapChainDesc.BufferCount = kSwapChainBufferCount;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// モニタに移したら、中身を破棄
	// スワップチェーンを生成
	hr = g_dxgiFactory->CreateSwapChainForHwnd(g_commandQueue, g_hwnd, &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&g_swapChain));
	assert(SUCCEEDED(hr));

	// RTV用ディスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeapDesc.NumDescriptors = kSwapChainBufferCount;
	hr = g_device->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&g_rtvDescriptorHeap));
	assert(SUCCEEDED(hr));

	// SwapChainResourceの生成とRTVの生成
	for (uint32_t i = 0; i < kSwapChainBufferCount; ++i) {
		// SwapChainからResourceを引っ張ってくる
		hr = g_swapChain->GetBuffer(i, IID_PPV_ARGS(&g_swapChainResource[i]));
		assert(SUCCEEDED(hr));
		// RTVの設定
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// ディスクリプタの先頭を取得
		g_rtvHandle[i] = g_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		// ディスクリプタハンドルをずらす
		g_rtvHandle[i].ptr += static_cast<size_t>(i) * g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		// RTVを生成
		g_device->CreateRenderTargetView(g_swapChainResource[i], &rtvDesc, g_rtvHandle[i]);
	}
}

void InitalizeDXC() {

	HRESULT hr = S_FALSE;

	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&g_dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&g_dxcCompiler));
	assert(SUCCEEDED(hr));

	hr = g_dxcUtils->CreateDefaultIncludeHandler(&g_includeHandler);
	assert(SUCCEEDED(hr));
}

void CreatePipelineState() {
	HRESULT hr = S_FALSE;

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	hr = g_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&g_rootSignature));
	assert(SUCCEEDED(hr));


	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
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
	IDxcBlob* vertexShaderBlob = CompilerShader(L"Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	IDxcBlob* pixelShaderBlob = CompilerShader(L"Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	// パイプライン生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = g_rootSignature;
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

	hr = g_device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&g_pipelineState));
	assert(SUCCEEDED(hr));

	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	vertexShaderBlob->Release();
	pixelShaderBlob->Release();
}

void CreateTriangle() {

	g_vertexResource = CreateBufferResource(sizeof(Vector4) * 3);

	// 頂点バッファビューの設定
	g_vertexBufferView.BufferLocation = g_vertexResource->GetGPUVirtualAddress();
	g_vertexBufferView.SizeInBytes = sizeof(Vector4) * 3;
	g_vertexBufferView.StrideInBytes = sizeof(Vector4);

	Vector4* vertexData = nullptr;
	g_vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	vertexData[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	vertexData[1] = { 0.0f,  0.5f, 0.0f, 1.0f };
	vertexData[2] = { 0.5f, -0.5f, 0.0f, 1.0f };
	g_vertexResource->Unmap(0, nullptr);

	g_materialBuffer.sizeInByte = sizeof(Vector4);
	g_materialBuffer.resource = CreateBufferResource(g_materialBuffer.sizeInByte);
	g_materialBuffer.resource->Map(0, nullptr, &g_materialBuffer.mapData);
	Vector4 materialData{ 1.0f,1.0f,0.0f,1.0f };
	memcpy(g_materialBuffer.mapData, &materialData, g_materialBuffer.sizeInByte);

	g_wvpBuffer.sizeInByte = sizeof(Matrix4x4);
	g_wvpBuffer.resource = CreateBufferResource(g_wvpBuffer.sizeInByte);
	g_wvpBuffer.resource->Map(0, nullptr, &g_wvpBuffer.mapData);
	Matrix4x4 worldMatrix = MakeAffineMatrix(g_transform.scale, g_transform.rotate, g_transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(g_cameraTransform.scale, g_cameraTransform.rotate, g_cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
	Matrix4x4 wvpData = worldMatrix * viewMatrix * projectionMatrix;
	memcpy(g_wvpBuffer.mapData, &wvpData, g_wvpBuffer.sizeInByte);
}

void MainLoop() {

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
			// これから書き込むバックバッファインデックスを取得
			uint32_t backBufferIndex = g_swapChain->GetCurrentBackBufferIndex();
			// TransitionBarrierの設定
			D3D12_RESOURCE_BARRIER barrier{};
			// 今回のバリアはTransition
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			// Noneにしておく
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			// バリアを張る対象のリソース
			barrier.Transition.pResource = g_swapChainResource[backBufferIndex];
			// PresentからRenderTargeへ
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			// TransitionBarrierを張る
			g_commandList->ResourceBarrier(1, &barrier);

			// 描画先のRTVを設定
			g_commandList->OMSetRenderTargets(1, &g_rtvHandle[backBufferIndex], false, nullptr);
			// 指定した色で画面全体をクリア
			float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
			g_commandList->ClearRenderTargetView(g_rtvHandle[backBufferIndex], clearColor, 0, nullptr);

			D3D12_VIEWPORT viewport{};
			viewport.Width = static_cast<float>(kClientWidth);
			viewport.Height = static_cast<float>(kClientHeight);
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			g_commandList->RSSetViewports(1, &viewport);

			D3D12_RECT scissorRect{};
			scissorRect.left = 0;
			scissorRect.right = kClientWidth;
			scissorRect.top = 0;
			scissorRect.bottom = kClientHeight;
			g_commandList->RSSetScissorRects(1, &scissorRect);

			//-----------------------------------------------------------------------------------------//

			g_transform.rotate.y += 0.03f;
			Matrix4x4 worldMatrix = MakeAffineMatrix(g_transform.scale, g_transform.rotate, g_transform.translate);
			Matrix4x4 cameraMatrix = MakeAffineMatrix(g_cameraTransform.scale, g_cameraTransform.rotate, g_cameraTransform.translate);
			Matrix4x4 viewMatrix = Inverse(cameraMatrix);
			Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
			Matrix4x4 wvpData = worldMatrix * viewMatrix * projectionMatrix;
			memcpy(g_wvpBuffer.mapData, &wvpData, g_wvpBuffer.sizeInByte);

			g_commandList->SetGraphicsRootSignature(g_rootSignature);
			g_commandList->SetPipelineState(g_pipelineState);
			g_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			g_commandList->IASetVertexBuffers(0, 1, &g_vertexBufferView);
			g_commandList->SetGraphicsRootConstantBufferView(0, g_materialBuffer.resource->GetGPUVirtualAddress());
			g_commandList->SetGraphicsRootConstantBufferView(1, g_wvpBuffer.resource->GetGPUVirtualAddress());
			g_commandList->DrawInstanced(3, 1, 0, 0);

			//-----------------------------------------------------------------------------------------//


			// RenderTargetからPresentへ
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			// TransitionBarrierを張る
			g_commandList->ResourceBarrier(1, &barrier);

			// コマンドリストの内容を確定
			hr = g_commandList->Close();
			assert(SUCCEEDED(hr));
			// GPUにコマンドリストの実行を行わせる
			ID3D12CommandList* commandLists[] = { g_commandList };
			g_commandQueue->ExecuteCommandLists(1, commandLists);
			// GPUとOSに画面交換を行うよう通知する
			g_swapChain->Present(1, 0);
			// Fenceの値を更新
			g_fenceValue++;
			// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
			hr = g_commandQueue->Signal(g_fence, g_fenceValue);
			assert(SUCCEEDED(hr));
			// Fenceの値が指定したSignal値にたどり着いているか確認する
			// GetCompletedValueの初期値はFence作成時に渡した初期値
			if (g_fence->GetCompletedValue() < g_fenceValue) {
				// 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
				g_fence->SetEventOnCompletion(g_fenceValue, g_fenceEvent);
				// イベントを待つ
				WaitForSingleObject(g_fenceEvent, INFINITE);
			}
			// 次フレーム用のコマンドリストを準備
			hr = g_commandAllocator->Reset();
			assert(SUCCEEDED(hr));
			hr = g_commandList->Reset(g_commandAllocator, nullptr);
			assert(SUCCEEDED(hr));
		}
	}
}

void Release() {
	g_wvpBuffer.resource->Release();
	g_materialBuffer.resource->Release();
	g_vertexResource->Release();
	g_pipelineState->Release();
	g_rootSignature->Release();
	g_swapChainResource[0]->Release();
	g_swapChainResource[1]->Release();
	g_rtvDescriptorHeap->Release();
	g_swapChain->Release();
	CloseHandle(g_fenceEvent);
	g_fence->Release();
	g_commandList->Release();
	g_commandAllocator->Release();
	g_commandQueue->Release();
	g_device->Release();
	g_dxgiFactory->Release();
	CloseWindow(g_hwnd);


	IDXGIDebug* debug = nullptr;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}
}

IDxcBlob* CompilerShader(const std::wstring& filePath, const wchar_t* profile) {
	Log(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile));

	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = S_FALSE;
	hr = g_dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
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
	hr = g_dxcCompiler->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		g_includeHandler,
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

ID3D12Resource* CreateBufferResource(size_t sizeInBytes) {
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
	HRESULT hr = g_device->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&result));
	assert(SUCCEEDED(hr));
	return result;
}
