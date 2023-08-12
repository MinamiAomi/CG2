#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include <memory>
#include <string>
#include <vector>

#include "../Math/MathUtils.h"

class Model {
public:
    struct Vertex {
        Vector3 position;
        Vector3 normal;
        Vector2 texcoord;
    };

    using Index = uint32_t;

    struct Mesh {
        std::string name;
        uint32_t baseVertexLocation;
        uint32_t startIndexLocation;
        uint32_t indexCount;
        uint32_t materialIndex_;
    };
    struct Material {
        std::string name;
        Vector3 albedo;
        Microsoft::WRL::ComPtr<Microsoft::WRL::ComPtr<ID3D12Resource>> constantBuffer_;
    };
    struct Texture {
        std::string name;
        Microsoft::WRL::ComPtr
    };
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_;

    std::vector<Mesh> meshes_;
    std::vector<Material> materials_;
    std::vector<Texture>

    std::vector<Vertex> vertices_;
    std::vector<Index> indices_;

};

class MaterialProperty {

};

class ModelInstance {
public:


private:

    Matrix4x4 worldMatrix_;
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;

    bool isActive_;           // 描画する 
    bool isTransparent_;    // 半透明描画
    bool castShadows_;      // 影を落とす
    bool receiveShadows_;   // 影を受ける
};