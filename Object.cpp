#include "Object.h"

#include "ObjFile.h"
#include "MeshResource.h"
#include "Material.h"
#include "GraphicsEngine.h"
#include "TextureManager.h"
#include "ImGuiManager.h"

namespace CG {

    GraphicsEngine* Object::graphicsEngine_ = nullptr;
    TextureManager* Object::textureManager_ = nullptr;

    void Object::StaticInitialize(GraphicsEngine* graphicsEngine, TextureManager* textureManager) {
        graphicsEngine_ = graphicsEngine;
        textureManager_ = textureManager;
    }

    void Object::Initialize(ObjFile* obj) {
        SetModel(obj);
        transformationBuffer_ = std::make_unique<DX12::DynamicBuffer>();
        transformationBuffer_->Initialize(graphicsEngine_->GetDevice(), sizeof(Matrix4x4) * 2);
    }

    void Object::UpdateTransform(const Matrix4x4& viewProjectionMatrix) {
        worldMatrix_ = Matrix4x4::MakeAffineTransform(scale, rotate, translate);
        Matrix4x4 transformationData[] = {
            worldMatrix_ * viewProjectionMatrix,
            worldMatrix_
        };
        memcpy(transformationBuffer_->GetDataBegin(), &transformationData, sizeof(transformationData));
    }

    void Object::Draw(DX12::CommandList& commandList) {
        commandList.GetCommandList()->SetGraphicsRootConstantBufferView(Material::kTransformation, transformationBuffer_->GetResource().GetGPUVirtualAddress());
        for (auto& mesh : meshes_) {
            auto& vertexBufferView = mesh.mesh->GetVertexBuffer().view.GetView();
            for (size_t i = 0; i < mesh.materials.size(); ++i) {
                mesh.materials[i]->SetGraphicsRoot(commandList);
                auto& indexBufferView = mesh.mesh->GetIndexBuffers()[i]->view;
                commandList.GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
                commandList.GetCommandList()->IASetIndexBuffer(&indexBufferView.GetView());
                commandList.GetCommandList()->DrawIndexedInstanced(indexBufferView.GetIndexCount(), 1, 0, 0, 0);
            }
        }
    }

    void Object::Edit(const std::string& label) {
        if (ImGui::TreeNode(label.c_str())) {
            ImGui::DragFloat3("Translate", &translate.x, 0.01f);
            Vector3 deg = rotate * Math::ToDegree;
            ImGui::DragFloat3("Rotate", &deg.x, 1.0f, -360.0f, 360.0f);
            rotate = deg * Math::ToRadian;
            ImGui::DragFloat3("Scale", &scale.x, 0.01f);

            for (size_t i = 0; i < meshes_.size(); ++i) {
                for (size_t j = 0; j < meshes_[i].materials.size(); ++j) {
                    meshes_[i].materials[j]->Edit("Material" + std::to_string(i * meshes_[i].materials.size() + j));
                }
            }
          
            ImGui::TreePop();
        }
    
    }

    void CG::Object::SetModel(ObjFile* obj) {
        auto& srcMeshList = obj->GetMeshes();
        meshes_.resize(srcMeshList.size());
        for (size_t i = 0; i < meshes_.size(); ++i) {
            auto& srcMesh = srcMeshList[i];
            auto& destMesh = meshes_[i];
            destMesh.mesh = &srcMesh->meshResource;
            destMesh.materials.resize(destMesh.mesh->GetIndexBuffers().size());
            for (size_t j = 0; j < destMesh.materials.size(); ++j) {
                auto& srcMaterial = srcMesh->materials[j];
                auto& destMaterial = destMesh.materials[j];
                destMaterial = std::make_unique<Material>();
                destMaterial->SetColor(srcMaterial->color);
                if (srcMaterial->texturePath.empty()) {
                    destMaterial->SetUseTexture(false);
                }
                else {
                    destMaterial->SetTextureHandle(textureManager_->LoadTexture(srcMaterial->texturePath));
                    destMaterial->SetUseTexture(true);
                }
                destMaterial->Initialize();
            }
        }
    }

}