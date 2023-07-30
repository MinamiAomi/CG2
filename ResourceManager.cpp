#include "ResourceManager.h"

#include "GraphicsEngine.h"
#include "Model.h"
#include "DX12/DX12.h"
#include "Utils.h"

namespace CG {

    void ResourceManager::Initialize(GraphicsEngine* graphicsEngine) {
        graphicsEngine_ = graphicsEngine;
    }

    std::shared_ptr<Model> ResourceManager::LoadModelFromObj(const std::string& directory, const std::string& name) {
        auto model = Model::CreateModel();
        model->LoadFromObj(*graphicsEngine_, directory, name);
        modelMap_[name] = model;
        return model;
    }

    std::shared_ptr<TextureSet> ResourceManager::LoadTextureFromPNG(const std::string& path) {
        auto texture = TextureSet::CreateTextureSet();
        auto  wpath = ConvertString(path);
        texture->texture_.InitializeFromPNG(graphicsEngine_->GetDevice(), graphicsEngine_->GetCommandQueue(), wpath);
        texture->view_.InitializeTexture2D(graphicsEngine_->GetDevice(), texture->texture_.GetResource(), graphicsEngine_->GetSRVDescriptorHeap().Allocate());
        textureMap_[path] = texture;
        return texture;
    }




}
