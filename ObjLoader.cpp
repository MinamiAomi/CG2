#include "ObjLoader.h"

#include <cassert>
#include <sstream>
#include <unordered_map>

void CG::Obj::Load(const std::filesystem::path& path) {
    directioryPath_ = path.parent_path().string();
    objFileName_ = path.filename().string();
    LoadObjFile();
}

void CG::Obj::LoadObjFile() {
    std::ifstream file(directioryPath_ + "/" + objFileName_);
    assert(file.is_open());

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    Mesh* currentMesh = nullptr;
    struct Vertex {
        Vector3 positions;
        Vector3 normals;
        Vector2 texcoords;
    };
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<std::string, uint32_t> vertexDefinitionMap;

    std::string line;
    while (std::getline(file, line)) {
        std::string identifier;
        std::istringstream iss(line);
        iss >> identifier;

        // コメント
        if (identifier == "#") {
            continue;
        }
        // mtlファイル
        else if (identifier == "mtllib") {
            iss >> mtlFileName_;
            LoadMTLFile();
        }
        // メッシュ
        else if (identifier == "o") {
            if (currentMesh) {



            }
            currentMesh = &meshes_.emplace_back();
            iss >> currentMesh->name;
        }
        // 座標
        else if (identifier == "v") {
            Vector3& position = positions.emplace_back();
            iss >> position.x >> position.y >> position.z;
        }
        // 法線
        else if (identifier == "vn") {
            Vector3& normal = normals.emplace_back();
            iss >> normal.x >> normal.y >> normal.z;
        }
        // テクスチャ座標
        else if (identifier == "vt") {
            Vector2 texcoord = texcoords.emplace_back();
            iss >> texcoord.x >> texcoord.y;
        }
        // 使用するマテリアル
        else if (identifier == "usemtl") {
            std::string materialName;
            iss >> materialName;
            assert(!materials_.empty());
            for (size_t i = 0; i < materials_.size(); ++i) {
                if (materials_[i].name == materialName) {
                    currentMesh->useMaterial = i;
                    break;
                }
            }
        }
        // インデックス
        else if (identifier == "f") {
            std::vector<std::string> vertexDefinitions;
            while (true) {
                std::string vertexDefinition;
                iss >> vertexDefinition;
                if (vertexDefinition.empty()) {
                    break;
                }
                vertexDefinitions.emplace_back(std::move(vertexDefinition));
            }

            // 三角面
            if (vertexDefinitions.size() == 3) {
                for (size_t i = 0; i < 3; ++i) {
                    if (vertexDefinitionMap.contains(vertexDefinitions[i])) {
                        indices.emplace_back(vertexDefinitionMap[vertexDefinitions[i]]);
                    }
                    else {
                        std::istringstream viss(vertexDefinitions[i]);
                        


                    }
                }
            }
            // 四角面
            else if (vertexDefinitions.size() == 4) {

            }
            // それ以外はエラー
            else {
                assert(false);
            }
        }


    }
}

void CG::Obj::LoadMTLFile() {
}



