#pragma once

#include <memory>
#include <vector>

// glad must be included before any other OpenGL header (GLFW, ...)
#include <glad/glad.h>

#include "core/engineContext.hpp"
#include "core/layer.hpp"
#include "core/timer.hpp"
#include "core/window.hpp"
#include "input/inputManager.hpp"
#include "render/renderTarget.hpp"
#include "render/renderer.hpp"
#include "scene/scene.hpp"
#include "textureManager.hpp"

// ---------------------------------------------------------------------------
// Application — top-level owner of the engine lifecycle.
//
// Owns the window, the shared services (input, textures, scene, renderer) and
// the stack of Layers. The behaviour itself lives in the layers.
//
// Usage:
//   Application app;
//   app.pushLayer(std::make_unique<GameLayer>());
//   app.run();
// ---------------------------------------------------------------------------
class Application
{
public:
    Application();
    ~Application();

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    void pushLayer(std::unique_ptr<Layer> layer);
    void run();

private:
    // Declaration order = construction order (services before the context).
    Window window_;
    Timer timer_;
    InputManager inputManager_;
    TextureManager textureManager_;
    Scene scene_;
    Renderer renderer_;
    ScreenTarget screenTarget_;

    EngineContext ctx_;

    // Destroyed first: layers may still reference the services above.
    std::vector<std::unique_ptr<Layer>> layers_;
};
