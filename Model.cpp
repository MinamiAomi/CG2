#include "Model.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <filesystem>

void Model::LoadFromObj(const std::string& directioryPath, const std::string& fileName) {
    meshes_.clear();
    materials_.clear();
    std::filesystem::path filePath(directioryPath + "/" + fileName);
    assert(filePath.extension() == ".obj");
    name_ = filePath.stem().string();
    LoadObjFile(directioryPath, fileName);

}

void Model::LoadObjFile(const std::string& directioryPath, const std::string& fileName) {
    std::vector<Vector4> positions;
    std::vector<Vector2> texcoords;
    std::vector<Vector3> normals;

    std::ifstream file(directioryPath + "/" + fileName);
    assert(file.is_open());

    std::string line;
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream iss(line);
        iss >> identifier;

        // コメントをスキップ
        if (identifier == "#") {
            continue;
        }
        // マテリアルを読み込み
        else if (identifier == "mtllib") {
            std::string materialFileName;
            iss >> materialFileName;
            LoadMaterialTemplateFile(directioryPath, materialFileName);
        }
        // メッシュを追加
        else if (identifier == "o") {
            std::string meshName;
            iss >> meshName;
            meshes_.emplace_back(meshName);
        }
        else if (identifier == "v") {
            Vector4& position = positions.emplace_back();
            iss >> position.x >> position.y >> position.z;
            position.w = 1.0f;
        }
        else if (identifier == "vn") {
            Vector3& normal = normals.emplace_back();
            iss >> normal.x >> normal.y >> normal.z;
        }
        else if (identifier == "vt") {
            Vector2& texcoord = texcoords.emplace_back();
            iss >> texcoord.x >> texcoord.y;
        }
        else if (identifier == "usemtl") {
            std::string materialName;
            iss >> materialName;
            assert(materials_.size() > 0);
            // マテリアル配列から探す
            auto materialIter = std::find_if(materials_.begin(), materials_.end(),
                [&](Material& material) {
                    return material.name == materialName;
                });
            // 
            assert(materialIter != materials_.end());
            meshes_.back().materialIndex = std::distance(materials_.begin(), materialIter);
        }
        else if (identifier == "f") {
            for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
                std::string vertexDefinition;
                iss >> vertexDefinition;

                std::istringstream viss(vertexDefinition);
                size_t elementIndices[3]{};
                for (int32_t element = 0; element < 3; ++element) {
                    std::string index;
                    std::getline(viss, index, '/');
                    elementIndices[element] = static_cast<size_t>(std::stoi(index));
                }
                Vector4 position = positions[elementIndices[0] - 1];
                Vector2 texcoord = texcoords[elementIndices[1] - 1];
                Vector3 normal = normals[elementIndices[2] - 1];

                meshes_.back().vertices.emplace_back(position, texcoord, normal);
            }
        }
    }
}

void Model::CreateModelResource() {
}

void Model::LoadMaterialTemplateFile(const std::string& directioryPath, const std::string& fileName) {
    std::ifstream file(directioryPath + "/" + fileName);
    assert(file.is_open());

    std::string line;
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream iss(line);
        iss >> identifier;

        // コメントをスキップ
        if (identifier == "#") {
            continue;
        }
        else if (identifier == "newmtl") {
            std::string materialName;
            iss >> materialName;
            materials_.emplace_back(materialName);
        }
        else if (identifier == "map_Kd") {
            std::string textureName;
            iss >> textureName;
            materials_.back().textureFilePath = directioryPath + "/" + textureName;
        }
        else if (identifier == "Kd") {
            Vector4& color = materials_.back().color;
            iss >> color.x >> color.y >> color.z;
            color.w = 1.0f;
        }
    }
}

Model::Object::~Object() {
    if (transformationResource_) {
        transformationResource_->Release();
        transformationResource_ = nullptr;
    }
}

Model::Material::~Material() {
    if (materialResource) {
        materialResource->Release();
        materialResource = nullptr;
    }
    if (textureResource) {
        textureResource->Release();
        textureResource = nullptr;
    }
}

Model::Mesh::~Mesh() {
    if (vertexResource) {
        vertexResource->Release();
        vertexResource = nullptr;
    }
}
