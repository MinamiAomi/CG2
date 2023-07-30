#pragma once

#include <vector>
#include <memory>

#include "DX12/DX12.h"
#include "Math/MathUtils.h"

namespace CG {

    class GraphicsEngine;
    class TextureManager;
    class ObjFile;
    class MeshResource;
    class Material;

    class Object {
    public:
        static void StaticInitialize(GraphicsEngine* graphicsEngine, TextureManager* textureManager);

        void Initialize(ObjFile* obj);
        void UpdateTransform(const Matrix4x4& viewProjectionMatrix);
        void Draw(DX12::CommandList& commandList);

        void Edit(const std::string& label);

        void SetModel(ObjFile* obj);
        
        Vector3 translate;
        Vector3 rotate;
        Vector3 scale{1.0f,1.0f,1.0f};

    private:
        struct MeshSet {
            MeshResource* mesh;
            std::vector<std::unique_ptr<Material>> materials;
        };

        static GraphicsEngine* graphicsEngine_;
        static TextureManager* textureManager_;

        Matrix4x4 worldMatrix_;

        std::vector<MeshSet> meshes_;
        std::unique_ptr<DX12::DynamicBuffer> transformationBuffer_;
    };

}