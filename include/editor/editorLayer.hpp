#pragma once

#include <memory>

#include "core/layer.hpp"
#include "editor/imguiBackend.hpp"
#include "editor/panels/inspectorPanel.hpp"
#include "render/renderTarget.hpp"

// ---------------------------------------------------------------------------
// EditorLayer — editor UI on top of the game.
//
// Redirects the game's rendering into an offscreen viewport, then shows that
// viewport and the debug panels with ImGui on the screen.
// Must be pushed after the GameLayer.
// ---------------------------------------------------------------------------
class EditorLayer final : public Layer
{
public:
    EditorLayer();

    void onAttach() override;
    void onDetach() override;
    void onUpdate() override;
    void onRender() override;

private:
    void setupDockSpace();
    void drawViewportPanel();

    std::unique_ptr<ImGuiBackend> imgui_;
    InspectorPanel inspectorPanel_;
    OffscreenTarget viewport_;
    RenderTarget *screenTarget_ = nullptr; // target to restore on detach

    // Size of the "Viewport" panel, applied to viewport_ at the next update
    // (avoids resizing the texture between the game render and its display).
    unsigned int requestedWidth_;
    unsigned int requestedHeight_;
    bool viewportHovered_ = false;
    bool dockLayoutChecked_ = false;
};
