#include "Shader.h"

#include <cassert>
#include <fstream>

namespace CG::DX12 {

    void Shader::Initialize(const void* data, size_t size, const std::string& name) {
        data_.resize(size);
        memcpy(&data_[0], data, size);
        name_ = name;
    }

    void Shader::Initialize(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
        assert(file);

        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        data_.resize(size);
        file.read(&data_[0], size);
        name_ = path.string();
    }

    void Shader::Output(const std::filesystem::path& path) {
        std::ofstream file(path, std::ios::out | std::ios::binary);
        assert(file);
        file.write(data_.data(), data_.size());
    }

}
