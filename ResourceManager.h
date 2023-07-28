#pragma once

#include <memory>
#include <string>

#include "DX12/DX12.h"

namespace CG {

    class Mesh;
    class Material;
    class Texture;

    class ResourceManager {
    public:
        void Initialize();

        void AddMesh();
        void AddMaterial();
        void AddTexture();

        Mesh* FindMesh(const std::string& name);
        Material* FindMaterial(const std::string& name);
        Texture* FindTexture(const std::string& name);

    private:

        std::vector<std::unique_ptr<Mesh>> meshes_;
        std::vector<std::unique_ptr<Material>> materials_;
        std::vector<std::unique_ptr<Texture>> textures_;
    };

}