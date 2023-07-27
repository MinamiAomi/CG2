#include "ResourceContainer.h"

namespace CG {

    MeshHandle ResourceContainer::FindMesh(const std::string& name) {
        for (size_t i = 0; i < meshes_.size(); ++i) {
            if (meshes_[i]->GetName() == name) {
                return { i };
            }
        }
        return {};
    }

    MaterialHandle ResourceContainer::FindMaterial(const std::string& name) {
        for (size_t i = 0; i < materials_.size(); ++i) {
            if (materials_[i]->GetName() == name) {
                return { i };
            }
        }
        return {};
    }

    TextureHandle ResourceContainer::FindTexture(const std::string& name) {
        for (size_t i = 0; i < textures_.size(); ++i) {
            if (textures_[i]->GetName() == name) {
                return { i };
            }
        }
        return {};
    }


}