#pragma once

#include <string>
#include <vector>
#include <memory>

#include "DX12/DX12.h"
#include "Math/MathUtils.h"
#include "MeshResource.h"

namespace CG {

    class GraphicsEngine;

    class ObjFile {
    public:
        struct Vertex {
            Vector3 position;
            Vector2 texcoord;
            Vector3 normal;
        };
        struct Material {
            std::string name;
            Vector4 color;
            std::string texturePath;
        };
        struct Mesh {
            std::string name;
            std::vector<Vertex> vertices;
            std::vector<std::vector<uint32_t>> indicesList;
            MeshResource meshResource;
            std::vector<Material*> materials;
        };

        static void StaticIntialize(GraphicsEngine* graphicsEngine);

        void LoadFromObj(const std::string& directioryPath, const std::string& fileName);

        const std::vector<std::unique_ptr<Mesh>>& GetMeshes() const { return meshes_; }
    private:
        static GraphicsEngine* graphicsEngine_;
        
        void LoadOBJFile(const std::string& directioryPath, const std::string& fileName);
        void LoadMTLFile(const std::string& directioryPath, const std::string& fileName);
        void LoadResource(GraphicsEngine& graphicsEngine);

        std::string name_;
        std::vector<std::unique_ptr<Mesh>> meshes_;
        std::vector<std::unique_ptr<Material>> materials_;
    };
}
