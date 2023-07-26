#include "ShaderCompiler.h"

#include <cassert>
#include <format>

#pragma comment(lib,"dxcompiler.lib")

namespace CG::DX12 {

    void ShaderCompiler::Initialize() {
        if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils_.GetAddressOf())))) {
            assert(false);
        }
        if (FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler_.GetAddressOf())))) {
            assert(false);
        }
        if (FAILED(utils_->CreateDefaultIncludeHandler(includeHandler_.GetAddressOf()))) {
            assert(false);
        }
    }

    Shader ShaderCompiler::Compile(const std::filesystem::path& path, const std::wstring& profile) {
        OutputDebugStringW(std::format(L"Begin CompileShader, path:{}, profile:{}\n", path.wstring(), profile).c_str());

        ComPtr<IDxcBlobEncoding> shaderSource;
        if (FAILED(utils_->LoadFile(path.c_str(), nullptr, shaderSource.GetAddressOf()))) {
            assert(false);
        }

        DxcBuffer shader_source_buffer{};
        shader_source_buffer.Ptr = shaderSource->GetBufferPointer();
        shader_source_buffer.Size = shaderSource->GetBufferSize();
        shader_source_buffer.Encoding = DXC_CP_UTF8;

        LPCWSTR arguments[] = {
            path.c_str(),
            L"-E", L"main",
            L"-T", profile.c_str(),
            L"-Zi", L"-Qembed_debug",
            L"-Od",
            L"-Zpr"
        };

        ComPtr<IDxcResult> shaderResult;
        if (FAILED(compiler_->Compile(
            &shader_source_buffer,
            arguments,
            _countof(arguments),
            includeHandler_.Get(),
            IID_PPV_ARGS(shaderResult.GetAddressOf())))) {
            assert(false);
        }

        ComPtr<IDxcBlobUtf8> shaderError = nullptr;
        if (FAILED(shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(shaderError.GetAddressOf()), nullptr))) {
            assert(false);
        }
        if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
            std::string errorMsg = shaderError->GetStringPointer();
            OutputDebugStringA(errorMsg.c_str());
            assert(false);
        }

        ComPtr<IDxcBlob> blob;
        if (FAILED(shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(blob.GetAddressOf()), nullptr))) {
            assert(false);
        }

        Shader shader;
        shader.Initialize(blob->GetBufferPointer(), blob->GetBufferSize(), path.string());

        OutputDebugStringW(std::format(L"Compile Succeeded, path:{}, profile:{}\n", path.wstring(), profile).c_str());
        return shader;
    }


}