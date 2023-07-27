#pragma once
#include "Resource.h"

#include <memory>
#include <vector>

#include "DX12/DX12.h"
#include "Math/MathUtils.h"

namespace CG {

    class Material;

    class Mesh : public Resource {
    public:
        void SetVertices(void* vertices, size_t vertexCount, size_t stride);
        void AddIndices(void* indices, size_t indexCount, size_t stride, Material* material);

        DX12::VertexBuffer& GetVertexBuffer() { return vertexBuffer_; }
        std::vector<std::unique_ptr<DX12::IndexBuffer>>& GetIndexBuffers() { return indexBuffers_; }
        std::vector<Material*>& GetMaterials() { return materials_; }

    private:
        DX12::VertexBuffer vertexBuffer_;
        std::vector<std::unique_ptr<DX12::IndexBuffer>> indexBuffers_;
        std::vector<Material*> materials_;
    };

}