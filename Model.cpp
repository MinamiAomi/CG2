#include "Model.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace CG {

    void Model::LoadFromObj(const std::string& directioryPath, const std::string& fileName) {
        meshes_.clear();
        materials_.clear();
        std::filesystem::path filePath(directioryPath + "/" + fileName);
        assert(filePath.extension() == ".obj");
        name_ = filePath.stem().string();
        LoadOBJFile(directioryPath, fileName);

    }

    void Model::LoadOBJFile(const std::string& directioryPath, const std::string& fileName) {
        std::vector<Vector3> positions;
        std::vector<Vector3> normals;
        std::vector<Vector2> texcoords;

        std::ifstream file(directioryPath + "/" + fileName);
        assert(file.is_open());

        name_ = fileName;
        std::string currentMeshName;
        std::unordered_map<std::string, uint32_t> vertexDefinitionMap;

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
                LoadMTLFile(directioryPath, materialFileName);
            }
            // メッシュを追加
            else if (identifier == "o") {
                iss >> currentMeshName;
                meshMap_[currentMeshName] = std::make_unique<Mesh>();
            }
            else if (identifier == "v") {
                Vector3& position = positions.emplace_back();
                iss >> position.x >> position.y >> position.z;
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
                assert(materialMap_.contains(materialName));
                // マテリアル配列から探す
                meshMap_[currentMeshName]->materials_.emplace_back(materialMap_[materialName].get());
                meshMap_[currentMeshName]->indicesList.emplace_back();
            }
            else if (identifier == "f") {
                // 面の頂点を取得
                std::vector<std::string> vertexDefinitions;
                while (true) {
                    std::string vertexDefinition;
                    iss >> vertexDefinition;
                    if (vertexDefinition.empty()) {
                        break;
                    }
                    vertexDefinitions.emplace_back(std::move(vertexDefinition));
                }
                assert(vertexDefinitions.size() > 2);
                std::vector<uint32_t> face(vertexDefinitions.size());
                for (uint32_t i = 0; i < vertexDefinitions.size(); ++i) {
                    // 頂点が登録済み
                    if (vertexDefinitionMap.contains(vertexDefinitions[i])) {
                        face[i] = vertexDefinitionMap[vertexDefinitions[i]];
                    }
                    else {
                        std::istringstream viss(vertexDefinitions[i]);
                        uint32_t elementIndices[3]{};
                        bool useElement[3]{};
                        for (uint32_t j = 0; j < 3; ++j) {
                            std::string index;
                            std::getline(viss, index, '/');
                            if (!index.empty()) {
                                elementIndices[j] = static_cast<uint32_t>(std::stoi(index)) - 1;
                                useElement[j] = true;
                            }
                        }
                        auto& vertex = meshMap_[currentMeshName]->vertices.emplace_back();
                        vertex.position = positions[elementIndices[0]];
                        if (useElement[1]) {
                            vertex.texcoord = texcoords[elementIndices[1]];
                        }
                        if (useElement[2]) {
                            vertex.normal = normals[elementIndices[2]];
                        }
                        face[i] = vertexDefinitionMap[vertexDefinitions[i]] = static_cast<uint32_t>(meshMap_[currentMeshName]->vertices.size());
                    }
                }

                for (uint32_t i = 0; i < face.size() - 2; ++i) {
                    auto& indices = meshMap_[currentMeshName]->indicesList.back();
                    indices.emplace_back(0);
                    indices.emplace_back(i + 1);
                    indices.emplace_back(i + 2);
                }
            }
        }
    }

    void Model::LoadMTLFile(const std::string& directioryPath, const std::string& fileName) {
        std::ifstream file(directioryPath + "/" + fileName);
        assert(file.is_open());

        std::string currentMaterialName;

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
                iss >> currentMaterialName;
                materialMap_[currentMaterialName] = std::make_unique<Material>();
            }
            else if (identifier == "map_Kd") {
                std::string textureName;
                iss >> textureName;
                textureMap_[textureName] = std::make_unique<Texture>();
                textureMap_[textureName]->filepath = directioryPath + "/" + textureName;
            }
            else if (identifier == "Kd") {
                Vector4& color = materialMap_[currentMaterialName]->color;
                iss >> color.x >> color.y >> color.z;
                color.w = 1.0f;
            }
        }
    }

    void Model::LoadResource(const DX12::Device& device, DX12::CommandQueue& commandQueue) {
        size_t vertexStrideSize = sizeof(Vertex);
        size_t indexStrideSize = sizeof(uint32_t);

        for (auto& iter : meshMap_) {
            auto& mesh = iter.second;
            size_t vertexBufferSize = mesh->vertices.size() * vertexStrideSize;
            mesh->vertexBuffer.resource.InitializeForBuffer(device, vertexBufferSize);
            void* vertexData = nullptr;
            vertexData = mesh->vertexBuffer.resource.Map();
            memcpy(vertexData, mesh->vertices.data(), vertexBufferSize);
            mesh->vertexBuffer.resource.Unmap();

            for (size_t indexIndex = 0; indexIndex < mesh->indicesList.size(); ++indexIndex) {
                size_t indexBufferSize = mesh->indicesList[indexIndex].size() * indexBufferSize;
                mesh->indexBuffers[indexIndex]->resource.InitializeForBuffer(device, indexBufferSize);
            }
        }

    }

}