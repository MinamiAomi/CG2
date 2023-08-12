#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <vector>

#include "../Math/MathUtils.h"

struct VertexPosNorTex {
    Vector3 position;
    Vector3 normal;
    Vector2 texcoord;
};

using VertexCollection = std::vector<VertexPosNorTex>;
using IndexCollection = std::vector<uint32_t>;

class Mesh {
public:



private:
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> indexBuffers_;

    VertexCollection vertices_;
    IndexCollection indices_;
};