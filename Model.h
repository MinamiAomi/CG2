#pragma once

#include <d3d12.h>

#include <string>
#include <vector>

#include "Math/MathUtils.h"

class Model {
public:
    class Object {
    public:
        Object(const Object&) = delete;
        const Object& operator=(const Object&) = delete;
        ~Object();

        void Draw();

    private:
        Object();

        const Model* model_ = nullptr;
        ID3D12Resource* transformationResource_ = nullptr;

        Vector3 translate_;
        Vector3 rotate_;
        Vector3 scale_;
        Matrix4x4 worldMatrix_;

        friend class Model;
    };

    void LoadFromObj(const std::string& directioryPath, const std::string& fileName);
    Object* CreateObject();

private:
    struct VertexData {
        Vector4 position;
        Vector2 texcoord;
        Vector3 normal;
    };

    struct Material {
        const std::string name;
        std::string textureFilePath;
        Vector4 color;
        ID3D12Resource* materialResource = nullptr;
        ID3D12Resource* textureResource = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE textureCPUHandle = {};
        D3D12_GPU_DESCRIPTOR_HANDLE textureGPUHandle = {};

        explicit Material(const std::string& name) : name(name) {};
        ~Material();
    };
    struct Mesh {
        const std::string name;
        std::vector<VertexData> vertices;
        size_t materialIndex = 0;
        ID3D12Resource* vertexResource = nullptr;

        explicit Mesh(const std::string& name) : name(name) {};
        ~Mesh();
    };

    void LoadObjFile(const std::string& directioryPath, const std::string& fileName);
    void LoadMaterialTemplateFile(const std::string& directioryPath, const std::string& fileName);
    void CreateModelResource();

    std::string name_;
    std::vector<Mesh> meshes_;
    std::vector<Material> materials_;
};
