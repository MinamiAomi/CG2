#pragma once

#include "DX12/DX12.h"
#include "Math/MathUtils.h"

namespace CG {

    enum class Format {
        UInt16,
        UInt32
    };

    class Mesh {
    public:

        class Vertex {
            Vector3 position;
            Vector2 texcoord;
            Vector3 normal;
            Vector3 tangent;
        };
        void SetVertices(const std::vector<Vertex>& vertices);

    private:
        std::vector<Vertex> vertices;
        void* indices;
        std::vector<uint32_t> indices;


    };

}