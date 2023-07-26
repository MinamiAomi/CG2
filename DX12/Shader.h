#pragma once

#include <d3d12.h>

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace CG::DX12 {

    class Shader {
    public:
        void Initialize(const void* data, size_t size, const std::string& name);
        void Initialize(const std::filesystem::path& path);

        void Output(const std::filesystem::path& path);

        const std::string& GetName() const { return name_; }
        const std::vector<char>& GetData() const { return data_; }
        const char* GetPointer() const { return data_.data(); }
        size_t GetSize() const { return data_.size(); }

    private:
        std::string name_;
        std::vector<char> data_;
    };

}