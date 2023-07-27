#pragma once
#include "Resource.h"

#include <filesystem>

#include "DX12/DX12.h"

namespace CG {

    class Texture : public Resource {
    public:
        void LoadFromPNG(const std::filesystem::path& path);

    private:
        DX12::Resource resource;
        DX12::ShaderResourceView view;
    };

}