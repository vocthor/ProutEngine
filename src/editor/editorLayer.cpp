#include "editor/editorLayer.hpp"

#include <algorithm>

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_internal.h> // DockBuilder API

#include "core/window.hpp"
#include "scene/scene.hpp"

namespace
{
    constexpr unsigned int DEFAULT_VIEWPORT_WIDTH = 1280;
    constexpr unsigned int DEFAULT_VIEWPORT_HEIGHT = 720;

    // Default layout (used when imgui.ini has no saved dock layout):
    // the inspector is docked on the right, the viewport fills the rest.
    constexpr float INSPECTOR_WIDTH_RATIO = 0.25f;
}

// ---------------------------------------------------------------------------
EditorLayer::EditorLayer()
    : viewport_(DEFAULT_VIEWPORT_WIDTH, DEFAULT_VIEWPORT_HEIGHT),
      requestedWidth_(DEFAULT_VIEWPORT_WIDTH),
      requestedHeight_(DEFAULT_VIEWPORT_HEIGHT)
{
}

void EditorLayer::onAttach()
{
    imgui_ = std::make_unique<ImGuiBackend>(ctx().window.handle());

    // The game now draws into the viewport instead of the screen.
    screenTarget_ = ctx().renderTarget;
    ctx().renderTarget = &viewport_;
}

void EditorLayer::onDetach()
{
    ctx().renderTarget = screenTarget_;
    ctx().gameInputEnabled = true;
    imgui_.reset();
}

// ---------------------------------------------------------------------------
void EditorLayer::onUpdate()
{
    // Runs after GameLayer::onUpdate and before GameLayer::onRender.
    viewport_.resize(requestedWidth_, requestedHeight_);
}

void EditorLayer::onRender()
{
    // Game has already been rendered into viewport_: draw the UI on the screen.
    screenTarget_->bind();
    ::glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    imgui_->beginFrame();

    setupDockSpace();

    drawViewportPanel();
    inspectorPanel_.draw(ctx().scene);

    // The game only gets input while the mouse is over the viewport.
    ctx().gameInputEnabled = viewportHovered_ && !::ImGui::GetIO().WantTextInput;

    imgui_->endFrame();
}

// ---------------------------------------------------------------------------
void EditorLayer::drawViewportPanel()
{
    ::ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ::ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    const ImVec2 avail = ::ImGui::GetContentRegionAvail();
    requestedWidth_ = static_cast<unsigned int>(std::max(avail.x, 1.0f));
    requestedHeight_ = static_cast<unsigned int>(std::max(avail.y, 1.0f));

    // OpenGL textures are bottom-up: flip V.
    ::ImGui::Image(static_cast<ImTextureID>(viewport_.colorTexture()),
                   ImVec2(static_cast<float>(viewport_.width()), static_cast<float>(viewport_.height())),
                   ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    // Invisible button over the image: it owns the clicks made on the viewport
    // (the title bar stays free for moving the window), and stays active while
    // the mouse is held, even if it leaves the image during a camera drag.
    ::ImGui::SetCursorScreenPos(::ImGui::GetItemRectMin());
    ::ImGui::InvisibleButton("##viewport_input", ::ImGui::GetItemRectSize(),
                             ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
    viewportHovered_ = ::ImGui::IsItemHovered() || ::ImGui::IsItemActive();

    ::ImGui::End();
    ::ImGui::PopStyleVar();
}

// ---------------------------------------------------------------------------
void EditorLayer::setupDockSpace()
{
    const ImGuiID dockspaceId = ::ImGui::DockSpaceOverViewport(0, ::ImGui::GetMainViewport());

    // Only build the default layout once, and only if imgui.ini did not restore one.
    if (dockLayoutChecked_)
        return;
    dockLayoutChecked_ = true;

    const ImGuiDockNode *node = ::ImGui::DockBuilderGetNode(dockspaceId);
    if (node != nullptr && node->IsSplitNode())
        return;

    const ImGuiViewport *main = ::ImGui::GetMainViewport();
    ::ImGui::DockBuilderRemoveNode(dockspaceId);
    ::ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
    ::ImGui::DockBuilderSetNodeSize(dockspaceId, main->WorkSize);

    ImGuiID inspectorId = 0;
    ImGuiID viewportId = 0;
    ::ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Right, INSPECTOR_WIDTH_RATIO, &inspectorId, &viewportId);
    ::ImGui::DockBuilderDockWindow("Viewport", viewportId);
    ::ImGui::DockBuilderDockWindow("Inspector", inspectorId);
    ::ImGui::DockBuilderFinish(dockspaceId);
}
