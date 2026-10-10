#pragma once

#include <memory>

#include "cameraController.hpp"
#include "core/layer.hpp"
#include "input/signal.hpp"
#include "model.hpp"
#include "render/shaderProgram.hpp"

// ---------------------------------------------------------------------------
// GameLayer — the game itself: builds the scene, moves the camera, submits the
// scene to the renderer. It draws into ctx().renderTarget, so it behaves the
// same whether that is the screen (game build) or an editor viewport.
// ---------------------------------------------------------------------------
class GameLayer final : public Layer
{
public:
    void onAttach() override;
    void onDetach() override;
    void onUpdate() override;
    void onRender() override;

private:
    // Entities in the scene point to these models: they must outlive the scene's use of them.
    std::unique_ptr<Model> backpackModel_;
    std::unique_ptr<Model> cubeModel_;
    std::unique_ptr<ShaderProgram> shaderProgram_;
    std::unique_ptr<CameraController> cameraController_;
    Connection escConn_;
};
