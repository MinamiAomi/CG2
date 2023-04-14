#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <cassert>
#include "StringUtils.h"

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
void CreateGameWindow(const std::string& windowName, uint32_t clientWidth, uint32_t clientHeight);
void InitalizeDirectX();
void CreateDirectXCommand();
void CreateScreenRenderTarget(uint32_t clientWidth, uint32_t clientHeight);

// 定数
constexpr uint32_t kSwapChainBufferCount = 2;

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


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// 出力ウィンドウに文字出力
	Log("Hello,DirectX!\n");

	// クライアント領域サイズ
	const uint32_t kClientWidth = 1280;
	const uint32_t kClientHeight = 720;

	CreateGameWindow("CG2", kClientWidth, kClientHeight);
	InitalizeDirectX();
	CreateDirectXCommand();
	CreateScreenRenderTarget(kClientWidth, kClientHeight);

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

	return 0;
}

void CreateGameWindow(const std::string& windowName, uint32_t clientWidth, uint32_t clientHeight) {
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
	RECT wrc{ 0,0,static_cast<LONG>(clientWidth),static_cast<LONG>(clientHeight) };
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
	D3D_FEATURE_LEVEL featureLevels[] =	{ 
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0 
	};
	const char* featureLevelStrings[] =	{ "12.2", "12.1", "12.0" };
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

void CreateScreenRenderTarget(uint32_t clientWidth, uint32_t clientHeight) {
	HRESULT hr = S_FALSE;
	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = clientWidth;		// 画面幅
	swapChainDesc.Height = clientHeight;	// 画面高
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
