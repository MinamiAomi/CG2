#include "Graphics.h"

#include "Defines.h"
#include "DescriptorAllocator.h"

namespace {

    Graphics* graphics = nullptr;

}

uint64_t Graphics::frameCount_ = 0;

void Graphics::Create() {
    if (!graphics) {
        graphics = new Graphics;
        graphics->Initialize();
    }
}

void Graphics::Destroy() {
    if (graphics) {
        delete graphics;
        graphics = nullptr;
    }
}

Graphics& Graphics::Get() {
    ASSERT(graphics);
    return *graphics;
}

DescriptorAllocation Graphics::AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors) {
    return descriptorAllocators_[type]->Allocate(numDescriptors);
}
