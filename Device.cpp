#include "Device.h"

#include <cassert>
#include <cstdint>
#include <format>

namespace CG::DX12 {

    void Device::Initialize() {
#ifdef _DEBUG
        // デバッグレイヤーを有効化する
        {
            ComPtr<ID3D12Debug1> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
                debugController->EnableDebugLayer();
                debugController->SetEnableGPUBasedValidation(TRUE);
            }
        }
#endif
        device_.Reset();
        adapter_.Reset();
        factory_.Reset();

        // DXGIファクトリーの生成
        if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(factory_.GetAddressOf())))) {
            assert(false);
        }

        // 良い順にアダプターを頼む
        for (uint32_t i = 0;
            factory_->EnumAdapterByGpuPreference(
                i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                IID_PPV_ARGS(adapter_.GetAddressOf())) != DXGI_ERROR_NOT_FOUND;
            ++i) {
            // アダプター情報を取得
            DXGI_ADAPTER_DESC3 adapterDesc{};
            if (FAILED(adapter_->GetDesc3(&adapterDesc))) {
                assert(false);
            }
            // ソフトウェアアダプタでなければ採用
            if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
                // 採用したアダプタ情報を出力
                OutputDebugStringW(std::format(L"Use Adapter:{}\n", adapterDesc.Description).c_str());
                break;
            }
            adapter_.Reset(); // ソフトウェアアダプタは見なかったことにする
        }
        assert(adapter_);


        // 機能レベルとログ出力用の文字列
        D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
        };
        const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };
        // 高い順に生成できるか試していく
        for (size_t i = 0; i < _countof(featureLevels); ++i) {
            // 採用したアダプターデバイスを生成
            // 指定した機能レベルでデバイスが生成できたかを確認
            if (SUCCEEDED(D3D12CreateDevice(
                adapter_.Get(), featureLevels[i], IID_PPV_ARGS(device_.GetAddressOf())))) {
                // 生成できたのでログ出力を行ってループを抜ける
                OutputDebugStringA(std::format("FeatureLevel : {}\n", featureLevelStrings[i]).c_str());
                break;
            }
        }

#ifdef _DEBUG
        {
            // デバッグ時のみ
            ComPtr<ID3D12InfoQueue> infoQueue;
            if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf())))) {
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
            }
        }
#endif
    }

}