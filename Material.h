#pragma once
#include "Resource.h"

#include "DX12/DX12.h"
#include "Math/MathUtils.h"

namespace CG {

    class Texture;

    class Material : public Resource {
    public:

    private:
        DX12::Resource constantBuffer_;
        Texture* texture_;
    };

}