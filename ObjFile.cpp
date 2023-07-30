#include "ObjFile.h"

#include <cstdint>
#include <cassert>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>

#include "Utils.h"
#include "GraphicsEngine.h"

namespace CG {

    GraphicsEngine* ObjFile::graphicsEngine_ = nullptr;

    void ObjFile::StaticIntialize(GraphicsEngine* graphicsEngine) {
        graphicsEngine_ = graphicsEngine;
    }
    void ObjFile::LoadFromObj(const std::string& directioryPath, const std::string& fileName) {
        meshes_.clear();
        materials_.clear();
        name_ = fileName;
        LoadOBJFile(directioryPath, fileName);
        LoadResource(*graphicsEngine_);
    }

    void ObjFile::CreateCube(const Vector3& size) {
        meshes_.clear();
        materials_.clear();


        const uint32_t kSurfaceCount = 6;
        const uint32_t kVertexCount = 24;
        const uint32_t kIndexCount = 36;
        std::vector<Vertex> vertices(kVertexCount);
        std::vector<uint32_t> indices(kIndexCount);

        enum { LTN, LBN, RTN, RBN, LTF, LBF, RTF, RBF, };

        Vector3 h = size * 0.5f;

        Vector3 position[] = {
            { -h.x,  h.y, -h.z }, // 左上前
            { -h.x, -h.y, -h.z }, // 左下前
            {  h.x,  h.y, -h.z }, // 右上前
            {  h.x, -h.y, -h.z }, // 右下前
            { -h.x,  h.y,  h.z }, // 左上奥
            { -h.x, -h.y,  h.z }, // 左下奥
            {  h.x,  h.y,  h.z }, // 右上奥
            {  h.x, -h.y,  h.z }, // 右下奥 
        };

        // 前面
        vertices[0].position = position[LTN];
        vertices[1].position = position[LBN];
        vertices[2].position = position[RTN];
        vertices[3].position = position[RBN];
        // 後面
        vertices[4].position = position[RTF];
        vertices[5].position = position[RBF];
        vertices[6].position = position[LTF];
        vertices[7].position = position[LBF];
        // 右面
        vertices[8].position = position[RTN];
        vertices[9].position = position[RBN];
        vertices[10].position = position[RTF];
        vertices[11].position = position[RBF];
        // 左面
        vertices[12].position = position[LTF];
        vertices[13].position = position[LBF];
        vertices[14].position = position[LTN];
        vertices[15].position = position[LBN];
        // 上面
        vertices[16].position = position[LTF];
        vertices[17].position = position[LTN];
        vertices[18].position = position[RTF];
        vertices[19].position = position[RTN];
        // 下面
        vertices[20].position = position[LBN];
        vertices[21].position = position[LBF];
        vertices[22].position = position[RBN];
        vertices[23].position = position[RBF];

        // 法線
        Vector3 normal[] = {
            Vector3::back,
            Vector3::forward,
            Vector3::right,
            Vector3::left,
            Vector3::up,
            Vector3::down,
        };

        for (size_t i = 0; i < kSurfaceCount; i++) {
            size_t j = i * 4;
            vertices[j + 0].normal = normal[i];
            vertices[j + 1].normal = normal[i];
            vertices[j + 2].normal = normal[i];
            vertices[j + 3].normal = normal[i];
        }
        for (size_t i = 0; i < kSurfaceCount; i++) {
            size_t j = i * 4;
            vertices[j + 0].texcoord = {0.0f,0.0f};
            vertices[j + 1].texcoord = {0.0f,1.0f};
            vertices[j + 2].texcoord = {1.0f,0.0f};
            vertices[j + 3].texcoord = {1.0f,1.0f};
        }
        // インデックス
        for (uint32_t i = 0, j = 0; j < kSurfaceCount; j++) {
            uint32_t k = j * 4;

            indices[i++] = k + 1;
            indices[i++] = k;
            indices[i++] = k + 2;
            indices[i++] = k + 1;
            indices[i++] = k + 2;
            indices[i++] = k + 3;
        }

        meshes_.emplace_back(std::make_unique<Mesh>());
        meshes_.back()->vertices = std::move(vertices);
        meshes_.back()->indicesList.emplace_back() = std::move(indices);
        materials_.emplace_back(std::make_unique<Material>());
        meshes_.back()->materials.emplace_back(materials_.back().get());
        meshes_.back()->materials.back()->color = { 1.0f,1.0f,1.0f,1.0f };
        LoadResource(*graphicsEngine_);
    }

    void ObjFile::CreateSphere(float radius, uint32_t subdivision) {
        meshes_.clear();
        materials_.clear();

        const float kLonEvery = Math::TwoPi / float(subdivision);
        const float kLatEvery = Math::Pi / float(subdivision);
        const uint32_t kLatVertexCount = subdivision + 1ull;
        const uint32_t kLonVertexCount = subdivision + 1ull;
        const uint32_t kVertexCount = kLonVertexCount * kLatVertexCount;
        const uint32_t kIndexCount = uint32_t(subdivision * subdivision) * 6;

        std::vector<Vertex> vertices(kVertexCount);
        std::vector<uint32_t> indices(kIndexCount);

        auto CalcPosition = [](float lat, float lon) {
            return Vector3{
                { std::cos(lat) * std::cos(lon) },
                { std::sin(lat) },
                { std::cos(lat) * std::sin(lon) }
            };
        };
        auto CalcTexcoord = [&](size_t latIndex, size_t lonIndex) {
            return Vector2{
                float(lonIndex) / float(subdivision),
                1.0f - float(latIndex) / float(subdivision)
            };
        };


        // 頂点
        for (uint32_t latIndex = 0; latIndex < kLonVertexCount; ++latIndex) {
            float lat = -Math::HalfPi + float(kLatEvery) * latIndex;

            for (uint32_t lonIndex = 0; lonIndex < kLatVertexCount; ++lonIndex) {
                float lon = lonIndex * kLonEvery;

                uint32_t vertexIndex = latIndex * kLatVertexCount + lonIndex;
                vertices[vertexIndex].position = CalcPosition(lat, lon) * radius;
                vertices[vertexIndex].texcoord = CalcTexcoord(latIndex, lonIndex);
                vertices[vertexIndex].normal = static_cast<Vector3>(vertices[vertexIndex].position);
            }
        }
        // インデックス
        for (uint32_t i = 0; i < subdivision; ++i) {
            uint32_t y0 = i * kLatVertexCount;
            uint32_t y1 = (i + 1) * uint32_t(kLatVertexCount);

            for (uint32_t j = 0; j < subdivision; ++j) {
                uint32_t index0 = y0 + j;
                uint32_t index1 = y1 + j;
                uint32_t index2 = y0 + j + 1;
                uint32_t index3 = y1 + j + 1;

                uint32_t indexIndex = (i * subdivision + j) * 6;
                indices[indexIndex++] = index0;
                indices[indexIndex++] = index1;
                indices[indexIndex++] = index2;
                indices[indexIndex++] = index1;
                indices[indexIndex++] = index3;
                indices[indexIndex++] = index2;
            }
        }
        meshes_.emplace_back(std::make_unique<Mesh>());
        meshes_.back()->vertices = std::move(vertices);
        meshes_.back()->indicesList.emplace_back() = std::move(indices);
        materials_.emplace_back(std::make_unique<Material>());
        meshes_.back()->materials.emplace_back(materials_.back().get());
        meshes_.back()->materials.back()->color = { 1.0f,1.0f,1.0f,1.0f };
        LoadResource(*graphicsEngine_);
    }

    void ObjFile::LoadOBJFile(const std::string& directioryPath, const std::string& fileName) {
        std::vector<Vector3> positions;
        std::vector<Vector3> normals;
        std::vector<Vector2> texcoords;

        std::ifstream file(directioryPath + "/" + fileName);
        assert(file.is_open());

        name_ = fileName;
        std::unordered_map<std::string, uint32_t> vertexDefinitionMap;
        size_t currentMeshIndex = 0;

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
                meshes_.emplace_back(std::make_unique<Mesh>());
                meshes_[currentMeshIndex]->name = meshName;
                currentMeshIndex = meshes_.size() - 1;
            }
            else if (identifier == "v") {
                Vector3& position = positions.emplace_back();
                iss >> position.x >> position.y >> position.z;
                position.z = -position.z;
            }
            else if (identifier == "vn") {
                Vector3& normal = normals.emplace_back();
                iss >> normal.x >> normal.y >> normal.z;
                normal.z = -normal.z;
            }
            else if (identifier == "vt") {
                Vector2& texcoord = texcoords.emplace_back();
                iss >> texcoord.x >> texcoord.y;
                texcoord.y = 1.0f - texcoord.y;
            }
            else if (identifier == "usemtl") {
                std::string materialName;
                iss >> materialName;
                // マテリアル配列から探す
                auto iter = std::find_if(materials_.begin(), materials_.end(),
                    [&](const auto& material) {return material->name == materialName; });
                if (iter != materials_.end()) {
                    if (meshes_.empty()) {
                        meshes_.emplace_back(std::make_unique<Mesh>());
                    }
                    meshes_[currentMeshIndex]->materials.emplace_back(iter->get());
                    meshes_[currentMeshIndex]->indicesList.emplace_back();
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
                        auto& vertex = meshes_[currentMeshIndex]->vertices.emplace_back();
                        vertex.position = positions[elementIndices[0]];
                        if (useElement[1]) {
                            vertex.texcoord = texcoords[elementIndices[1]];
                        }
                        if (useElement[2]) {
                            vertex.normal = normals[elementIndices[2]];
                        }
                        face[i] = vertexDefinitionMap[vertexDefinitions[i]] = static_cast<uint32_t>(meshes_[currentMeshIndex]->vertices.size() - 1);
                    }
                }

                for (uint32_t i = 0; i < face.size() - 2; ++i) {
                    auto& indices = meshes_[currentMeshIndex]->indicesList.back();
                    indices.emplace_back(face[i + 2]);
                    indices.emplace_back(face[i + 1]);
                    indices.emplace_back(face[0]);
                }
            }
        }
    }

    void ObjFile::LoadMTLFile(const std::string& directioryPath, const std::string& fileName) {
        std::ifstream file(directioryPath + "/" + fileName);
        assert(file.is_open());

        size_t currentMaterialIndex = 0;

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
                materials_.emplace_back(std::make_unique<Material>());
                currentMaterialIndex = materials_.size() - 1;
                materials_[currentMaterialIndex]->name = materialName;
            }
            else if (identifier == "map_Kd") {
                std::string textureName;
                iss >> textureName;
                materials_[currentMaterialIndex]->texturePath = directioryPath + "/" + textureName;
            }
            else if (identifier == "Kd") {
                Vector4& color = materials_[currentMaterialIndex]->color;
                iss >> color.x >> color.y >> color.z;
                color.w = 1.0f;
            }
        }
    }

    void ObjFile::LoadResource(GraphicsEngine& graphicsEngine) {
        size_t vertexStrideSize = sizeof(Vertex);
        size_t indexStrideSize = sizeof(uint32_t);

        for (auto& iter : meshes_) {
            auto& mesh = *iter;
            auto& vertexBuffer = mesh.meshResource.vertexBuffer_;
            auto& vertices = mesh.vertices;
            size_t vertexBufferSize = vertices.size() * vertexStrideSize;
            vertexBuffer.resource.InitializeForBuffer(graphicsEngine.GetDevice(), vertexBufferSize);
            void* vertexData = nullptr;
            vertexData = vertexBuffer.resource.Map();
            memcpy(vertexData, vertices.data(), vertexBufferSize);
            vertexBuffer.resource.Unmap();
            vertexBuffer.view.Initialize(vertexBuffer.resource, vertexBufferSize, vertexStrideSize, vertices.size());

            for (size_t indexIndex = 0; indexIndex < mesh.indicesList.size(); ++indexIndex) {
                auto& indexBuffer = mesh.meshResource.indexBuffers_.emplace_back();
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
