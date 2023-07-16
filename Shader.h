#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

namespace CG::DX12 {

    class Shader {
    public:
        void Initialize(const char* data, size_t size);
        void Initialize(const std::filesystem::path& path);

        const std::vector<char>& GetData() const { return data_; }
        const char* GetPointer() const { return data_.data(); }
        size_t GetSize() const { return data_.size(); }

    private:
        std::vector<char> data_;
    };

}