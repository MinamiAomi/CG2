#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <deque>
#include <memory>
#include <string>

#include "Defines.h"

class UploadBuffer {
public:
    struct Allocation {
        void* cpu;
        D3D12_GPU_VIRTUAL_ADDRESS gpu;
    };

    explicit UploadBuffer(size_t pageSize = UNIT_MB(2));
    virtual ~UploadBuffer();

    size_t GetPageSize() const { return pageSize_; }

    Allocation Allocate(size_t sizeInByte, size_t alignment);

    void Reset();

private:
    struct Page {
        Page(size_t sizeInByte);
        ~Page();

        // 要求スペースがあるか調べる
        bool HasSpace(size_t sizeInByte, size_t alignment) const;

        Allocation Allocate(size_t sizeInByte, size_t alignment);

        void Reset();

    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
        void* cpuPtr_;
        D3D12_GPU_VIRTUAL_ADDRESS gpuPtr_;
        size_t pageSize_;
        size_t offset_;
    };

    using PagePool = std::deque<std::shared_ptr<Page>>;

    std::shared_ptr<Page> RequestPage();

    PagePool pagePool_;
    PagePool availablePages_;

    std::shared_ptr<Page> currentPage_;

    size_t pageSize_;
};
