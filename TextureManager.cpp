#include "TextureManager.h"

#include <cassert>

#include "GraphicsEngine.h"
#include "Utils.h"
#include "ImGuiManager.h"

namespace CG {
    TextureManager* TextureManager::GetInstance() {
        static TextureManager instance;
        return &instance;
    }
    void TextureManager::Initialize(GraphicsEngine* graphicsEngine) {
        graphicsEngine_ = graphicsEngine;
    }

    uint32_t TextureManager::LoadTexture(const std::string& name) {
        uint32_t handle{};

        auto iter = std::find_if(std::begin(textures_), std::end(textures_),
            [&](const auto& texture) {
                if (texture) {
                    return texture->name == name;
                }
                return false;
            });
        if (iter != std::end(textures_)) {
            handle = static_cast<uint32_t>(std::distance(std::begin(textures_), iter));
            return handle;
        }

        handle = nextLoadTextureIndex_;
        textures_[handle] = std::make_unique<TextureSet>();
        auto& texture = *textures_[handle];
        texture.name = name;
        std::wstring wname = ConvertString(name);
        texture.texture_.InitializeFromPNG(graphicsEngine_->GetDevice(), graphicsEngine_->GetCommandQueue(), wname);
        texture.view_.InitializeTexture2D(graphicsEngine_->GetDevice(), texture.texture_.GetResource(), graphicsEngine_->GetSRVDescriptorHeap().Allocate());
        ++nextLoadTextureIndex_;
        return handle;
    }
    void TextureManager::SetRootDescriptorTable(DX12::CommandList& commandList, uint32_t rootParameterIndex, uint32_t textureHandle) {
        assert(textures_[textureHandle]);
        commandList.GetCommandList()->SetGraphicsRootDescriptorTable(rootParameterIndex, textures_[textureHandle]->view_.GetDescriptor().GetGPUHandle());
    }
    uint32_t TextureManager::Edit(const std::string& label, uint32_t currentTextureHandle) {
        if (ImGui::BeginCombo(label.c_str(), textures_[currentTextureHandle]->name.c_str())) {

            for (uint32_t i = 0; i < nextLoadTextureIndex_; ++i) {
                const bool isSelected = (i == currentTextureHandle);
                if (ImGui::Selectable(textures_[i]->name.c_str(), isSelected)) {
                    currentTextureHandle = i;
                    break;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
        return currentTextureHandle;
    }
}