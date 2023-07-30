#include <Windows.h>

#include <cstdint>
#include <cassert>
#include <memory>
#include <map>
#include <vector>


#include "Math/MathUtils.h"
#include "Window.h"
#include "GraphicsEngine.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "ObjFile.h"
#include "Input.hpp"
#include "Material.h"
#include "Camera.h"
#include "Object.h"

struct DirectionalLightConstantData {
    Vector4 color;
    Vector3 direction;
    float intensity;
};

CG::DX12::D3DResourceLeakChecker leakChecker;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    {
        CG::Window* window = CG::Window::GetInstance();
        window->Initialize("Game", 1280, 720);

        CG::GraphicsEngine* graphicsEngine = CG::GraphicsEngine::GetInstance();
        graphicsEngine->Initialize(window);

        CG::TextureManager* textureManager = CG::TextureManager::GetInstance();
        textureManager->Initialize(graphicsEngine);

        Input::Initialize(window->GetHWND());

        CG::ImGuiManager* imguiManager = CG::ImGuiManager::GetInstance();
        imguiManager->Initialize(window, graphicsEngine);

        CG::ObjFile::StaticIntialize(graphicsEngine);
        CG::Material::StaticInitialize(graphicsEngine, textureManager);
        CG::Object::StaticInitialize(graphicsEngine, textureManager);

        std::string directory = "Resources";
        std::string loadFileName;
        std::map<std::string, std::unique_ptr<CG::ObjFile>> objMap;
        auto LoadObj = [&](const std::string& filename) {
            if (!objMap.contains(filename)) {
                auto object = std::make_unique<CG::ObjFile>();
                object->LoadFromObj(directory, filename);
                objMap[filename] = std::move(object);
            }
        };
        LoadObj("axis.obj");
        LoadObj("bunny.obj");
        LoadObj("suzanne.obj");
        LoadObj("multiMesh.obj");
        LoadObj("multiMaterial.obj");


        std::string previewObj;
        std::vector<std::unique_ptr<CG::Object>> objects;

        DirectionalLightConstantData light{};
        light.color = Vector4::one;
        light.direction = -Vector3::unitY;
        light.intensity = 1.0f;
        CG::DX12::DynamicBuffer lightBuffer;
        lightBuffer.Initialize(graphicsEngine->GetDevice(), sizeof(light));
        memcpy(lightBuffer.GetDataBegin(), &light, sizeof(light));

        Camera camera;
        camera.SetProjectionMatrix(45.0f * Math::ToRadian, float(window->GetClientWidth()) / window->GetClientHeight(), 0.1f, 1000.0f);

        window->Show();

        while (window->ProcessMessage()) {
            Input::Update();
            imguiManager->NewFrame();

            ImGui::Begin("Setting");
            char buf[256] = {};
            memcpy(buf, directory.data(), std::max(directory.size(), 256ull));
            if (ImGui::InputText("Directory", buf, 256)) {
                directory = buf;
            }
            memcpy(buf, loadFileName.data(), std::max(loadFileName.size(), 256ull));
            if (ImGui::InputText("FileName", buf, 256)) {
                loadFileName = buf;
            }
            if (ImGui::Button("Load")) {
                LoadObj(loadFileName);
                previewObj = loadFileName;
            }
            ImGui::Separator();
            if (ImGui::BeginCombo("Model", previewObj.c_str())) {
                for (auto& obj : objMap) {
                    const bool isSelectabled = (obj.first == previewObj);
                    if (ImGui::Selectable(obj.first.c_str(), &isSelectabled)) {
                        previewObj = obj.first;
                    }
                    if (isSelectabled) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Create")) {
                if (!previewObj.empty()) {
                    std::unique_ptr<CG::Object> object = std::make_unique<CG::Object>();
                    object->Initialize(objMap[previewObj].get());
                    objects.emplace_back(std::move(object));
                }
            }


            for (size_t i = 0; i < objects.size(); ++i) {
                ImGui::Separator();
                objects[i]->Edit("Object" + std::to_string(i));
            }


            ImGui::End();

            camera.Update();

            Matrix4x4 viewProjection = camera.GetViewMatrix() * camera.GetProjectionMatrix();
            for (auto& object : objects) {
                object->UpdateTransform(viewProjection);
            }

            graphicsEngine->PreDraw();
            auto& commandList = graphicsEngine->GetCommandList();
            CG::Material::DrawSetting(commandList, lightBuffer.GetResource().GetGPUVirtualAddress());

            for (auto& object : objects) {
                object->Draw(commandList);
            }

            imguiManager->Render(commandList);
            graphicsEngine->PostDraw();
        }

        imguiManager->Finalize();
        window->Finalize();
    }

    return 0;
}