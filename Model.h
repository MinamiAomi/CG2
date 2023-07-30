#pragma once

#include <string>
#include <vector>
#include <memory>

#include "DX12/DX12.h"
#include "Math/MathUtils.h"

namespace CG {

    class GraphicsEngine;

    class Model {
        friend class ResourceManager;
    public:

        struct Vertex {
            Vector3 position;
            Vector3 normal;
            Vector2 texcoord;
        };
        struct Material {
            std::string name;
            Vector4 color;
            DX12::DynamicBuffer constantBuffer;
            std::string texturePath;
        };
        struct Mesh {
            std::string name;
            std::vector<Vertex> vertices;
            std::vector<std::vector<uint32_t>> indicesList;
            DX12::VertexBuffer vertexBuffer;
            std::vector<std::unique_ptr<DX12::IndexBuffer>> indexBuffers;
            std::vector<Material*> materials_;
        };

        const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const { return meshes_; }

    private:
        static std::shared_ptr<Model> CreateModel() { return std::shared_ptr<Model>(new Model()); }
        Model() = default;
        Model(const Model&) = delete;
        const Model& operator=(const Model&) = delete;

        void LoadFromObj(GraphicsEngine& graphicsEngine, const std::string& directioryPath, const std::string& fileName);

        void LoadOBJFile(const std::string& directioryPath, const std::string& fileName);
        void LoadMTLFile(const std::string& directioryPath, const std::string& fileName);
        void LoadResource(GraphicsEngine& graphicsEngine);

        std::string name_;
        std::vector<std::unique_ptr<Mesh>> meshes_;
        std::vector<std::unique_ptr<Material>> materials_;
    };
}
