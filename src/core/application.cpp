#include "core/application.hpp"

#include <glad/glad.h>

namespace
{
    constexpr unsigned int WINDOW_WIDTH = 1920;
    constexpr unsigned int WINDOW_HEIGHT = 1080;
}

// ---------------------------------------------------------------------------
Application::Application()
    : window_(WINDOW_WIDTH, WINDOW_HEIGHT, "ProutEngine"),
      inputManager_(window_.handle()),
      scene_(Camera(static_cast<int>(WINDOW_WIDTH), static_cast<int>(WINDOW_HEIGHT), glm::vec3(0.0f, 0.0f, 2.0f))),
      renderer_(scene_, textureManager_),
      screenTarget_(window_),
      ctx_{.window = window_,
           .timer = timer_,
           .inputManager = inputManager_,
           .textureManager = textureManager_,
           .scene = scene_,
           .renderer = renderer_,
           .renderTarget = &screenTarget_}
{
    ::glEnable(GL_DEPTH_TEST);
    ::glDepthFunc(GL_LESS);
    ::glEnable(GL_STENCIL_TEST);
}

Application::~Application()
{
    for (auto it = layers_.rbegin(); it != layers_.rend(); ++it)
        (*it)->detach();
    layers_.clear();
}

// ---------------------------------------------------------------------------
void Application::pushLayer(std::unique_ptr<Layer> layer)
{
    layer->attach(ctx_);
    layers_.push_back(std::move(layer));
}

// ---------------------------------------------------------------------------
void Application::run()
{
    while (!window_.shouldClose())
    {
        timer_.update();
        inputManager_.update();

        for (auto &layer : layers_)
            layer->onUpdate();

        for (auto &layer : layers_)
            layer->onRender();

        window_.swapBuffers();
        Window::pollEvents();
    }
}
