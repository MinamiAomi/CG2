#pragma once

#include <d3d12.h>
#include <dxcapi.h>
#include <filesystem>
#include <wrl/client.h>

#include "Shader.h"

namespace CG::DX12 {
    template<class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    class ShaderCompiler {
    public:
        void Initialize();
        Shader Compile(const std::filesystem::path& path, const std::wstring& profile);

    private:
        ComPtr<IDxcUtils> utils_;
        ComPtr<IDxcCompiler3> compiler_;
        ComPtr<IDxcIncludeHandler> includeHandler_;
    };

}