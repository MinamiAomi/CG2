#include "Shader.h"

#include <cassert>
#include <fstream>

namespace CG::DX12 {

    void Shader::Initialize(const char* data, size_t size) {
        data_.resize(size);
        memcpy(&data_[0], data, size);
    }

    void Shader::Initialize(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
        assert(file);

        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        data_.resize(size);
        file.read(&data_[0], size);
    }

}
