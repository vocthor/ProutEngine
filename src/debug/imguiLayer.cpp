#include "debug/imguiLayer.hpp"

#include <cfloat>
#include <cmath>
#include <string>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "scene/entity.hpp"
#include "scene/light.hpp"
#include "scene/scene.hpp"

ImGuiLayer::ImGuiLayer(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ::ImGui::CreateContext();
    ::ImGui::StyleColorsDark();
    ::ImGui_ImplGlfw_InitForOpenGL(window, false);
    ::ImGui_ImplOpenGL3_Init("#version 330");
}

ImGuiLayer::~ImGuiLayer()
{
    ::ImGui_ImplOpenGL3_Shutdown();
    ::ImGui_ImplGlfw_Shutdown();
    ::ImGui::DestroyContext();
}

void ImGuiLayer::beginFrame()
{
    ::ImGui_ImplOpenGL3_NewFrame();
    ::ImGui_ImplGlfw_NewFrame();
    ::ImGui::NewFrame();
}

bool ImGuiLayer::wantsMouseCapture() const
{
    return ::ImGui::GetIO().WantCaptureMouse;
}

bool ImGuiLayer::wantsKeyboardCapture() const
{
    return ::ImGui::GetIO().WantCaptureKeyboard;
}

void ImGuiLayer::renderDebugPanel(Scene &scene)
{
    if (!debugGuiEnabled_)
        return;

    ::ImGui::Begin("ProutEngine Debug", &debugGuiEnabled_);

    ::ImGui::Text("FPS: %.1f", ::ImGui::GetIO().Framerate);
    ::ImGui::Separator();

    if (::ImGui::CollapsingHeader("Scene", ::ImGuiTreeNodeFlags_DefaultOpen))
    {
        ::ImGui::ColorEdit3("Ambient", &scene.ambientColor.x);

        ::ImGui::SeparatorText("Camera");
        ::ImGui::DragFloat3("Cam Position", &scene.camera.position.x, 0.05f);
        ::ImGui::DragFloat3("Cam Direction", &scene.camera.direction.x, 0.01f, -1.0f, 1.0f);

        const float dirLen2 = scene.camera.direction.x * scene.camera.direction.x +
                              scene.camera.direction.y * scene.camera.direction.y +
                              scene.camera.direction.z * scene.camera.direction.z;
        if (dirLen2 > 0.0001f)
        {
            const float invLen = 1.0f / std::sqrt(dirLen2);
            scene.camera.direction.x *= invLen;
            scene.camera.direction.y *= invLen;
            scene.camera.direction.z *= invLen;
        }

        ::ImGui::DragFloat("Cam Sensitivity", &scene.camera.sensitivity, 0.001f, 0.001f, 1.0f, "%.3f");
    }

    if (::ImGui::CollapsingHeader("Entities", ::ImGuiTreeNodeFlags_DefaultOpen))
    {
        ::ImGui::Text("Count: %d", static_cast<int>(scene.entities.size()));

        if (!scene.entities.empty())
        {
            if (selectedEntityIndex_ < 0)
                selectedEntityIndex_ = 0;
            if (selectedEntityIndex_ >= static_cast<int>(scene.entities.size()))
                selectedEntityIndex_ = static_cast<int>(scene.entities.size()) - 1;

            if (::ImGui::BeginListBox("Entity List", ImVec2(-FLT_MIN, 160.0f)))
            {
                for (int i = 0; i < static_cast<int>(scene.entities.size()); ++i)
                {
                    const bool isSelected = (selectedEntityIndex_ == i);
                    const char *entityName = scene.entities[i].name.empty() ? "<unnamed>" : scene.entities[i].name.c_str();
                    if (::ImGui::Selectable(entityName, isSelected))
                        selectedEntityIndex_ = i;
                    if (isSelected)
                        ::ImGui::SetItemDefaultFocus();
                }
                ::ImGui::EndListBox();
            }

            if (selectedEntityIndex_ >= 0 && selectedEntityIndex_ < static_cast<int>(scene.entities.size()))
            {
                Entity &entity = scene.entities[static_cast<std::size_t>(selectedEntityIndex_)];
                ::ImGui::SeparatorText("Selected Entity");
                ::ImGui::Text("Name: %s", entity.name.c_str());

                ::ImGui::DragFloat3("Position", &entity.transform.position.x, 0.05f);
                ::ImGui::DragFloat3("Rotation", &entity.transform.rotation.x, 0.5f);
                ::ImGui::DragFloat3("Scale", &entity.transform.scale.x, 0.01f, 0.001f, 100.0f);

                if (::ImGui::Button("Reset Transform"))
                {
                    entity.transform.position = {0.0f, 0.0f, 0.0f};
                    entity.transform.rotation = {0.0f, 0.0f, 0.0f};
                    entity.transform.scale = {1.0f, 1.0f, 1.0f};
                }
            }
        }
    }

    if (::ImGui::CollapsingHeader("Lights", ::ImGuiTreeNodeFlags_DefaultOpen))
    {
        ::ImGui::Text("Count: %d", static_cast<int>(scene.lights.size()));
        for (int i = 0; i < static_cast<int>(scene.lights.size()); ++i)
        {
            Light &light = scene.lights[static_cast<std::size_t>(i)];
            ::ImGui::PushID(i);

            const char *typeLabel = "POINT";
            switch (light.type)
            {
            case LightType::DIRECTIONAL:
                typeLabel = "DIRECTIONAL";
                break;
            case LightType::POINT:
                typeLabel = "POINT";
                break;
            case LightType::SPOT:
                typeLabel = "SPOT";
                break;
            }

            std::string header = std::string("Light ") + std::to_string(i) + " (" + typeLabel + ")";
            if (::ImGui::TreeNode(header.c_str()))
            {
                ::ImGui::ColorEdit3("Color", &light.color.x);
                ::ImGui::DragFloat("Intensity", &light.intensity, 0.01f, 0.0f, 50.0f);

                if (light.type != LightType::DIRECTIONAL)
                    ::ImGui::DragFloat3("Position", &light.position.x, 0.05f);
                if (light.type != LightType::POINT)
                {
                    ::ImGui::DragFloat3("Direction", &light.direction.x, 0.01f, -1.0f, 1.0f);

                    const float ldirLen2 = light.direction.x * light.direction.x +
                                           light.direction.y * light.direction.y +
                                           light.direction.z * light.direction.z;
                    if (ldirLen2 > 0.0001f)
                    {
                        const float invLen = 1.0f / std::sqrt(ldirLen2);
                        light.direction.x *= invLen;
                        light.direction.y *= invLen;
                        light.direction.z *= invLen;
                    }
                }

                if (light.type != LightType::DIRECTIONAL)
                {
                    ::ImGui::DragFloat("Constant", &light.constant, 0.001f, 0.0f, 2.0f, "%.3f");
                    ::ImGui::DragFloat("Linear", &light.linear, 0.001f, 0.0f, 2.0f, "%.3f");
                    ::ImGui::DragFloat("Quadratic", &light.quadratic, 0.001f, 0.0f, 2.0f, "%.3f");
                }

                if (light.type == LightType::SPOT)
                {
                    ::ImGui::DragFloat("CutOff", &light.cutOff, 0.1f, 0.0f, 90.0f, "%.1f deg");
                    ::ImGui::DragFloat("OuterCutOff", &light.outerCutOff, 0.1f, 0.0f, 90.0f, "%.1f deg");
                    if (light.outerCutOff < light.cutOff)
                        light.outerCutOff = light.cutOff;
                }

                ::ImGui::TreePop();
            }

            ::ImGui::PopID();
        }
    }

    ::ImGui::End();
}

void ImGuiLayer::endFrame()
{
    ::ImGui::Render();
    ::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
}
