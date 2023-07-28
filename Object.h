#pragma once

#include "DX12/DX12.h"
#include "Math/MathUtils.h"

namespace CG {

    class Object {
    public:


    private:
        Vector3 translate_;
        Vector3 rotate_;
        Vector3 scale_;
        Matrix4x4 worldMatrix_;

        DX12::DynamicBuffer transformationBuffer_;
        class Model* model_;
    };

}