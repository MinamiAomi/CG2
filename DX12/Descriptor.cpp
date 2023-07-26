#include "Descriptor.h"

#include "DescriptorHeap.h"

namespace CG::DX12 {

    Descriptor::Data::~Data() {
        descriptorHeap->Deallocate(this);
    }
}