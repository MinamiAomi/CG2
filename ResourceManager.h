#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "DX12/DX12.h"

namespace CG {

    class GraphicsEngine;
    class Model;

    class TextureSet {
        friend class ResourceManager;
    public:

        const DX12::Texture& GetTexture() const { return texture_; }
        const DX12::ShaderResourceView& GetView() const { return view_; }

    private:
        static std::shared_ptr<TextureSet> CreateTextureSet() { return std::shared_ptr<TextureSet>(new TextureSet); }

        TextureSet() = default;
        TextureSet(const TextureSet&) = delete;
        const TextureSet& operator=(const TextureSet&) = delete;

        DX12::Texture texture_;
        DX12::ShaderResourceView view_;
    };

    class ResourceManager {
    public:
        void Initialize(GraphicsEngine* graphicsEngine);

        std::shared_ptr<Model> LoadModelFromObj(const std::string& directory, const std::string& name);
        std::shared_ptr<TextureSet> LoadTextureFromPNG(const std::string& path);

        const std::shared_ptr<Model>& FindModel(const std::string& name) { return modelMap_[name]; }
        const std::shared_ptr<TextureSet>& FindTexture(const std::string& name) { return textureMap_[name]; }

    private:
        GraphicsEngine* graphicsEngine_{ nullptr };
        std::unordered_map<std::string, std::shared_ptr<Model>> modelMap_;
        std::unordered_map<std::string, std::shared_ptr<TextureSet>> textureMap_;
    };

}