#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "DX12/DX12.h"
#include "Math/MathUtils.h"

namespace CG {

    class Model {
    public:
        void LoadFromObj(const std::string& directioryPath, const std::string& fileName);

    private:
        struct Vertex {
            Vector3 position;
            Vector3 normal;
            Vector2 texcoord;
        };
        struct Texture {
            std::string filepath;
            DX12::Texture texture;
        };
        struct Material {
            Vector4 color;
            DX12::Resource buffer;
            Texture* texture_;
        };
        struct Mesh {
            std::vector<Vertex> vertices;
            std::vector<std::vector<uint32_t>> indicesList;
            DX12::VertexBuffer vertexBuffer;
            std::vector<std::unique_ptr<DX12::IndexBuffer>> indexBuffers;
            std::vector<Material*> materials_;
        };

        void LoadOBJFile(const std::string& directioryPath, const std::string& fileName);
        void LoadMTLFile(const std::string& directioryPath, const std::string& fileName);
        void LoadResource(const DX12::Device& device, DX12::CommandQueue& commandQueue);

        std::string name_;
        std::unordered_map<std::string, std::unique_ptr<Mesh>> meshMap_;
        std::unordered_map<std::string, std::unique_ptr<Material>> materialMap_;
        std::unordered_map<std::string, std::unique_ptr<Texture>> textureMap_;
    }
}
