#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

#include "Math/MathUtils.h"

namespace CG {

    class Obj {
    public:
        void Load(const std::filesystem::path& path);

    private:
        struct Face {
            uint32_t index[3][3];
        };
        struct Mesh {
            std::string name;
  

            size_t useMaterial;
        };
        struct Material {
            std::string name;
            Vector3 ambient;
            Vector3 diffuse;
            Vector3 specular;
            float shininess;
            Vector2 uvOffset;
            Vector2 uvScale;
            std::string texture;
        };

        void LoadObjFile();
        void LoadMTLFile();

        std::string directioryPath_;
        std::string objFileName_;
        std::string mtlFileName_;
        std::vector<Mesh> meshes_;
        std::vector<Material> materials_;
    };

}