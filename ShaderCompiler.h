#pragma once

#include <Windows.h>
#include <dxcapi.h>
#include <wrl.h>

#include <string>

class ShaderCompiler {
public:
    void Initalize();
    Microsoft::WRL::ComPtr<IDxcBlob> Compile(const std::wstring& filePath, const wchar_t* profile);

private:
    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_{ nullptr };
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_{ nullptr };
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_{ nullptr };
};