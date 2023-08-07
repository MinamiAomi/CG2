#pragma once

#include <string>

#include "DX12/DX12.h"

namespace CG {

    class GraphicsEngine;

    class TextureManager {
    public:
        static const uint32_t kMaxTextureCount = 256;
        static TextureManager* GetInstance();

        void Initialize(GraphicsEngine* graphicsEngine);

        uint32_t LoadTexture(const std::string& name);
        void SetRootDescriptorTable(DX12::CommandList& commandList, uint32_t rootParameterIndex, uint32_t textureHandle);

        uint32_t Edit(const std::string& label, uint32_t currentTextureHandle);

    private:
        struct TextureSet {
            DX12::Texture texture_;
            DX12::ShaderResourceView view_;
            std::string name;
        };

        GraphicsEngine* graphicsEngine_{ nullptr };
        std::unique_ptr<TextureSet> textures_[kMaxTextureCount];
        uint32_t nextLoadTextureIndex_;
    };

}