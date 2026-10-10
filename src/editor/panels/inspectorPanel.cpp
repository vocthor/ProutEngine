#include "editor/panels/inspectorPanel.hpp"

#include <cfloat>
#include <cmath>
#include <string>

#include <imgui.h>

#include "scene/entity.hpp"
#include "scene/light.hpp"
#include "scene/scene.hpp"

void InspectorPanel::draw(Scene &scene)
{
    ::ImGui::Begin("Inspector");

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
            if (selectedEntity_ < 0)
                selectedEntity_ = 0;
            if (selectedEntity_ >= static_cast<int>(scene.entities.size()))
                selectedEntity_ = static_cast<int>(scene.entities.size()) - 1;

            if (::ImGui::BeginListBox("Entity List", ImVec2(-FLT_MIN, 160.0f)))
            {
                for (int i = 0; i < static_cast<int>(scene.entities.size()); ++i)
                {
                    const bool isSelected = (selectedEntity_ == i);
                    const char *entityName = scene.entities[i].name.empty() ? "<unnamed>" : scene.entities[i].name.c_str();
                    if (::ImGui::Selectable(entityName, isSelected))
                        selectedEntity_ = i;
                    if (isSelected)
                        ::ImGui::SetItemDefaultFocus();
                }
                ::ImGui::EndListBox();
            }

            if (selectedEntity_ >= 0 && selectedEntity_ < static_cast<int>(scene.entities.size()))
            {
                Entity &entity = scene.entities[static_cast<std::size_t>(selectedEntity_)];
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
