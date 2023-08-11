#pragma once

#include <cstdint>
#include <vector>

#include "Texture.h"

class RenderTarget {
public:
    enum AttachmentPoint {
        Color0,
        Color1,
        Color2,
        Color3,
        Color4,
        Color5,
        Color6,
        Color7,
        DepthStencil,

        NumAttachMentPoints
    };

    RenderTarget();

    RenderTarget(const RenderTarget& copy) = default;
    RenderTarget& operator=(const RenderTarget& copy) = default;

    RenderTarget(RenderTarget&& move) = default;
    RenderTarget& operator=(RenderTarget&& move) = default;

    void AttachTexture(AttachmentPoint attachmentPoint, const Texture& texture);
    void Resize(uint32_t width, uint32_t height);

    const Texture& GetTexture(AttachmentPoint attachmentPoint) const { return textures_[attachmentPoint]; }
    const std::vector<Texture>& GetTextures() const { return textures_; }

    D3D12_RT_FORMAT_ARRAY GetRebderTargetFormat() const;
    DXGI_FORMAT GetDepthStencilFormat() const;

private:
    std::vector<Texture> textures_;
};