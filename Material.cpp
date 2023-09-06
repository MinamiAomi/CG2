#include "Material.h"

#include "GraphicsEngine.h"
#include "TextureManager.h"
#include "ImGuiManager.h"


namespace CG {

    struct MaterialData {
        Vector4 color;
        int32_t useTexture;
        int32_t padding[3];
        Matrix4x4 uvTransform;
    };

    GraphicsEngine* Material::graphicsEngine_ = nullptr;
    TextureManager* Material::textureManager_ = nullptr;
    DX12::RootSignature Material::rootSignature_;
    DX12::PipelineState Material::pipelineStates_[Material::kLightingCount];

    void Material::StaticInitialize(GraphicsEngine* graphicsEngine, TextureManager* textureManager) {
        graphicsEngine_ = graphicsEngine;
        textureManager_ = textureManager;
        DX12::ShaderCompiler shaderCompiler;
        shaderCompiler.Initialize();

        DX12::RootSignatureDesc rsDesc;
        rsDesc.AddDescriptor(DX12::DescriptorType::CBV, 0, DX12::ShaderVisibility::Vertex);
        rsDesc.AddDescriptor(DX12::DescriptorType::CBV, 0, DX12::ShaderVisibility::Pixel);
        rsDesc.AddDescriptor(DX12::DescriptorType::CBV, 1, DX12::ShaderVisibility::Pixel);
        rsDesc.AddDescriptorTable(DX12::ShaderVisibility::Pixel);
        rsDesc.AddDescriptorRange(DX12::RangeType::SRV, 0, 1);
        rsDesc.AddStaticSampler(0, DX12::ShaderVisibility::Pixel);
        rsDesc.AddFlag(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        rootSignature_.Initialize(graphicsEngine_->GetDevice(), rsDesc);


        DX12::Shader vs = shaderCompiler.Compile("Object3d.VS.hlsl", L"vs_6_0");
        DX12::Shader gs = shaderCompiler.Compile("TestGS.hlsl", L"gs_6_0");
        DX12::Shader ps[kLightingCount]{
            shaderCompiler.Compile("NoneLighting.PS.hlsl", L"ps_6_0"),
            shaderCompiler.Compile("LambertLighting.PS.hlsl", L"ps_6_0"),
            shaderCompiler.Compile("HalfLambertLighting.PS.hlsl", L"ps_6_0"),
        };

        DX12::GraphicsPipelineStateDesc psDesc;
        for (size_t i = 0; i < kLightingCount; ++i) {
            psDesc.Clear();
            psDesc.SetRootSignature(rootSignature_);
            psDesc.SetVertexShader(vs);
            psDesc.SetGeometryShader(gs);
            //psDesc.SetPixelShader(ps[i]);
            psDesc.AddInputElementVertex("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0);
            psDesc.AddInputElementVertex("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0);
            psDesc.AddInputElementVertex("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0);
            psDesc.SetPrimitiveTopologyType(CG::DX12::PrimitiveTopology::Triangle);
            psDesc.SetRasterizerState(CG::DX12::CullMode::Back, CG::DX12::FillMode::Solid);
            //psDesc.AddRenderTargetState(CG::DX12::BlendMode::Normal, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
            psDesc.SetDepthState(DXGI_FORMAT_D24_UNORM_S8_UINT);
            psDesc.SetSampleState();
            pipelineStates_[i].Initialize(graphicsEngine->GetDevice(), psDesc);
        }

    }

    void Material::DrawSetting(DX12::CommandList& commandList, D3D12_GPU_VIRTUAL_ADDRESS lightBuffer) {
        auto cmdList = commandList.GetCommandList();
        cmdList->SetGraphicsRootSignature(rootSignature_.GetRootSignature().Get());
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmdList->SetGraphicsRootConstantBufferView(kLight, lightBuffer);
    }

    void Material::Initialize() {
        constantBuffer_.Initialize(graphicsEngine_->GetDevice(), sizeof(MaterialData));
        TransferData();
    }

    void Material::TransferData() {
        MaterialData data{};
        data.color = color_;
        data.useTexture = useTexture_ ? 1 : 0;
        data.uvTransform = Matrix4x4::MakeAffineTransform(Vector3(uvScale_), { 0.0f,0.0f,uvRotate_ }, Vector3(uvOffset_));
        memcpy(constantBuffer_.GetDataBegin(), &data, sizeof(data));
    }

    void Material::SetGraphicsRoot(DX12::CommandList& commandList) {
        auto cmdList = commandList.GetCommandList();
        cmdList->SetPipelineState(pipelineStates_[lighting_].GetPipelineState().Get());
        cmdList->SetGraphicsRootConstantBufferView(kMaterial, constantBuffer_.GetResource().GetGPUVirtualAddress());
        if (useTexture_) {
            textureManager_->SetRootDescriptorTable(commandList, kTexture, textureHandle_);
        }
    }

    void Material::Edit(const std::string& label) {
        if (ImGui::TreeNode(label.c_str())) {
            ImGui::ColorEdit4("Color", &color_.x);
            ImGui::DragFloat2("UV offset", &uvOffset_.x, 0.01f);
            float deg = uvRotate_ * Math::ToDegree;
            ImGui::DragFloat("UV rotate", &deg, 1.0f, -360.0f, 360.0f);
            uvRotate_ = deg * Math::ToRadian;
            ImGui::DragFloat2("UV scale", &uvScale_.x, 0.01f);
            
            ImGui::Checkbox("Use texture", &useTexture_);
            textureHandle_ = textureManager_->Edit("Texture", textureHandle_);

            const char* items[] = {"None", "Lambert", "Half lambert"};
            if (ImGui::BeginCombo("Lighting", items[lighting_])) {
                for (size_t i = 0; i < kLightingCount; ++i) {
                    
                    if (ImGui::Selectable(items[i])) {
                        lighting_ = static_cast<Lighting>(i);
                        break;
                    }
                }
                ImGui::EndCombo();
            }
            TransferData();
            ImGui::TreePop();
        }
    
    }

}