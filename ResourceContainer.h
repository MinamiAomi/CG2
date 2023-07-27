#pragma once

#include <filesystem>
#include <memory>
#include <vector>
#include <unordered_map>

#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

namespace CG {

    class ResourceContainer;

    struct MeshHandle { size_t index; };
    struct MaterialHandle { size_t index; };
    struct TextureHandle { size_t index; };
    struct Model {
        std::vector<MeshHandle> meshes;
        std::vector<MaterialHandle> materials;
        std::vector<TextureHandle> textures;
    };
    class ResourceContainer {
    public:

        Model LoadFromObj(const std::filesystem::path& path);

        MeshHandle FindMesh(const std::string& name);
        MaterialHandle FindMaterial(const std::string& name);
        TextureHandle FindTexture(const std::string& name);

        Mesh& GetMesh(MeshHandle handle) { return *meshes_[handle.index]; }
        Material& GetMaterial(MaterialHandle handle) { return *materials_[handle.index]; }
        Texture& GetTexture(MeshHandle handle) { return *textures_[handle.index]; }

    private:
        std::vector<std::unique_ptr<Mesh>> meshes_;
        std::vector<std::unique_ptr<Material>> materials_;
        std::vector<std::unique_ptr<Texture>> textures_;
    };

}