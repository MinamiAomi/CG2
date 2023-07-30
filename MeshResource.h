#pragma once

#include "DX12/DX12.h"
#include "Math/MathUtils.h"

namespace CG {

    class Material;

    class MeshResource {
        friend class ObjFile;
    public:
        const DX12::VertexBuffer& GetVertexBuffer() { return vertexBuffer_; }
        const std::vector<std::unique_ptr<DX12::IndexBuffer>>& GetIndexBuffers() { return indexBuffers_; }

    private:
        DX12::VertexBuffer vertexBuffer_;
        std::vector<std::unique_ptr<DX12::IndexBuffer>> indexBuffers_;
    };

}