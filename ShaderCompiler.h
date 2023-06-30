#pragma once

#include <Windows.h>
#include <dxcapi.h>

#include <string>

class ShaderCompiler {
public:
    void Initalize();
    IDxcBlob* Compile(const std::wstring& filePath, const wchar_t* profile);

private:
    IDxcUtils* dxcUtils_{ nullptr };
    IDxcCompiler3* dxcCompiler_{ nullptr };
    IDxcIncludeHandler* includeHandler_{ nullptr };
};