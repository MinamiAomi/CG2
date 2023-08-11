#include "RenderTarget.h"

RenderTarget::RenderTarget() :
    textures_(AttachmentPoint::NumAttachMentPoints) {
}

void RenderTarget::AttachTexture(AttachmentPoint attachmentPoint, const Texture& texture) {
    textures_[attachmentPoint] = texture;
}

void RenderTarget::Resize(uint32_t width, uint32_t height) {
    for (auto& texture : textures_) {
        texture.Resize(width, height);
    }
}

D3D12_RT_FORMAT_ARRAY RenderTarget::GetRebderTargetFormat() const {
    D3D12_RT_FORMAT_ARRAY rtvFormats{};
    for (int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i) {
        const auto& texture = textures_[i];
        if (texture.IsValid()) {
            rtvFormats.RTFormats[rtvFormats.NumRenderTargets++] = texture.GetResourceDesc().Format;
        }
    }
    return rtvFormats;
}

DXGI_FORMAT RenderTarget::GetDepthStencilFormat() const {
    DXGI_FORMAT dsvFormat = DXGI_FORMAT_UNKNOWN;
    const auto& texture = textures_[AttachmentPoint::DepthStencil];
    if (texture.IsValid()) {
        dsvFormat = texture.GetResourceDesc().Format;
    }
    return dsvFormat;
}
