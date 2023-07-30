#include "Model.h"

#include <cassert>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>

#include "Utils.h"
#include "GraphicsEngine.h"

namespace CG {

    void Model::LoadFromObj(GraphicsEngine& graphicsEngine, const std::string& directioryPath, const std::string& fileName) {
        meshes_.clear();
        materials_.clear();
        name_ = fileName;
        LoadOBJFile(directioryPath, fileName);
        LoadResource(graphicsEngine);
    }

    void Model::LoadOBJFile(const std::string& directioryPath, const std::string& fileName) {
        std::vector<Vector3> positions;
        std::vector<Vector3> normals;
        std::vector<Vector2> texcoords;

        std::ifstream file(directioryPath + "/" + fileName);
        assert(file.is_open());

        name_ = fileName;
        std::unordered_map<std::string, uint32_t> vertexDefinitionMap;
        Mesh* currentMesh = nullptr;

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
                std::string meshName;
                iss >> meshName;
                currentMesh = meshes_.emplace_back(std::make_unique<Mesh>()).get();
                currentMesh->name = meshName;
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
                // マテリアル配列から探す
                auto iter = std::find_if(materials_.begin(), materials_.end(),
                    [&](const auto& material) {return material->name == materialName; });
                if (iter != materials_.end()) {
                    currentMesh->materials_.emplace_back(iter->get());
                    currentMesh->indicesList.emplace_back();
                }
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
                        auto& vertex = currentMesh->vertices.emplace_back();
                        vertex.position = positions[elementIndices[0]];
                        if (useElement[1]) {
                            vertex.texcoord = texcoords[elementIndices[1]];
                        }
                        if (useElement[2]) {
                            vertex.normal = normals[elementIndices[2]];
                        }
                        face[i] = vertexDefinitionMap[vertexDefinitions[i]] = static_cast<uint32_t>(currentMesh->vertices.size() - 1);
                    }
                }

                for (uint32_t i = 0; i < face.size() - 2; ++i) {
                    auto& indices = currentMesh->indicesList.back();
                    indices.emplace_back(face[0]);
                    indices.emplace_back(face[i + 1]);
                    indices.emplace_back(face[i + 2]);
                }
            }
        }
    }

    void Model::LoadMTLFile(const std::string& directioryPath, const std::string& fileName) {
        std::ifstream file(directioryPath + "/" + fileName);
        assert(file.is_open());

        Material* currentMaterial = nullptr;

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
                currentMaterial = materials_.emplace_back(std::make_unique<Material>()).get();
                currentMaterial->name = materialName;
            }
            else if (identifier == "map_Kd") {
                std::string textureName;
                iss >> textureName;
                currentMaterial->texturePath = directioryPath + "/" + textureName;
            }
            else if (identifier == "Kd") {
                Vector4& color = currentMaterial->color;
                iss >> color.x >> color.y >> color.z;
                color.w = 1.0f;
            }
        }
    }

    void Model::LoadResource(GraphicsEngine& graphicsEngine) {
        size_t vertexStrideSize = sizeof(Vertex);
        size_t indexStrideSize = sizeof(uint32_t);

        for (auto& iter : meshes_) {
            auto& mesh = *iter;
            auto& vertexBuffer = mesh.vertexBuffer;
            auto& vertices = mesh.vertices;
            size_t vertexBufferSize = vertices.size() * vertexStrideSize;
            vertexBuffer.resource.InitializeForBuffer(graphicsEngine.GetDevice(), vertexBufferSize);
            void* vertexData = nullptr;
            vertexData = vertexBuffer.resource.Map();
            memcpy(vertexData, vertices.data(), vertexBufferSize);
            vertexBuffer.resource.Unmap();
            vertexBuffer.view.Initialize(vertexBuffer.resource, vertexBufferSize, vertexStrideSize, vertices.size());

            for (size_t indexIndex = 0; indexIndex < mesh.indicesList.size(); ++indexIndex) {
                auto& indexBuffer = mesh.indexBuffers.emplace_back();
                indexBuffer = std::make_unique<CG::DX12::IndexBuffer>();
                auto& indices = mesh.indicesList[indexIndex];
                size_t indexBufferSize = indices.size() * indexStrideSize;
                indexBuffer->resource.InitializeForBuffer(graphicsEngine.GetDevice(), indexBufferSize);
                void* indexData = nullptr;
                indexData = indexBuffer->resource.Map();
                memcpy(indexData, mesh.indicesList[indexIndex].data(), indexBufferSize);
                indexBuffer->resource.Unmap();
                indexBuffer->view.Initialize(indexBuffer->resource, indexBufferSize, indices.size());
            }
        }
    }
}
