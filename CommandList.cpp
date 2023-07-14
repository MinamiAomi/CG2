#include "CommandList.h"

#include <cassert>

namespace CG::DX12 {

    CommandList::CommandList() :
        fenceEvent_(nullptr),
        fenceValue_(0) {
    }

    CommandList::~CommandList() {
        if (IsEnabled()) {
            WaitForGPU();
            CloseHandle(fenceEvent_);
        }
    }

    void CommandList::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device, Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue) {
        assert(device);
        assert(commandQueue);

        commandQueue_ = commandQueue;
        
        HRESULT hr = S_FALSE;

        // コマンドアロケータを生成
        hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator_.GetAddressOf()));
        assert(SUCCEEDED(hr));

        // コマンドリストを生成
        hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(commandList_.GetAddressOf()));
        assert(SUCCEEDED(hr));

        fenceValue_ = 0;

        // フェンスを生成
        hr = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.GetAddressOf()));
        assert(SUCCEEDED(hr));

        fenceEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
        assert(fenceEvent_ != nullptr);

    }

    void CommandList::ExcuteCommand() {
        // コマンドリストの内容を確定
        HRESULT hr = commandList_->Close();
        assert(SUCCEEDED(hr));
        // GPUにコマンドリストの実行を行わせる
        ID3D12CommandList* commandLists[] = { commandList_.Get() };
        commandQueue_->ExecuteCommandLists(1, commandLists);
    }

    void CommandList::WaitForGPU() {
        // Fenceの値を更新
        ++fenceValue_;
        // GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
        HRESULT hr = commandQueue_->Signal(fence_.Get(), fenceValue_);
        assert(SUCCEEDED(hr));
        // Fenceの値が指定したSignal値にたどり着いているか確認する
              // GetCompletedValueの初期値はFence作成時に渡した初期値
        if (fence_->GetCompletedValue() < fenceValue_) {
            // 指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
            fence_->SetEventOnCompletion(fenceValue_, fenceEvent_);
            // イベントを待つ
            WaitForSingleObject(fenceEvent_, INFINITE);
        }
    }

    void CommandList::Reset() {
        // 次フレーム用のコマンドリストを準備
        HRESULT hr = commandAllocator_->Reset();
        assert(SUCCEEDED(hr));
        hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
        assert(SUCCEEDED(hr));
    }

}