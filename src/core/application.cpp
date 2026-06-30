#include "core/application.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "cameraController.hpp"
#include "input/inputManager.hpp"
#include "modelBuilder.hpp"
#include "render/material.hpp"
#include "render/renderer.hpp"
#include "render/shader.hpp"
#include "render/shaderProgram.hpp"
#include "scene/light.hpp"
#include "scene/scene.hpp"
#include "textureManager.hpp"
#include "utils/log.hpp"

// ---------------------------------------------------------------------------
Application::Application()
    : window_(800, 600, "ProutEngine")
{
    ::glEnable(GL_DEPTH_TEST);
    ::glDepthFunc(GL_LESS);
    ::glEnable(GL_STENCIL_TEST);
}

// ---------------------------------------------------------------------------
void Application::run()
{
    // --- Resources ----------------------------------------------------------
    TextureManager textureManager;

    Model backpackModel =
        ModelBuilder::load("resources/models/backpack/backpack.obj", textureManager).build();

    // --- Scene --------------------------------------------------------------
    Scene scene(Camera(static_cast<int>(window_.width()),
                       static_cast<int>(window_.height()),
                       glm::vec3(0.0f, 0.0f, 2.0f)));

    scene.entities.push_back(Entity{.name = "Backpack", .model = &backpackModel});

    scene.ambientColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    scene.lights = {
        // Directional (blueish moonlight)
        {.type = LightType::DIRECTIONAL,
         .color = {0.4f, 0.4f, 1.0f},
         .intensity = 0.6f,
         .direction = {-0.2f, -1.0f, -0.3f}},
        // Green point light
        {.type = LightType::POINT,
         .color = {0.25f, 1.0f, 0.25f},
         .intensity = 1.0f,
         .position = {0.7f, 0.2f, 2.0f}},
        // Red point light
        {.type = LightType::POINT,
         .color = {1.0f, 0.25f, 0.25f},
         .intensity = 1.0f,
         .position = {2.3f, -3.3f, -4.0f}},
        // Spotlight — flashlight attached to camera, updated every frame
        {.type = LightType::SPOT,
         .color = {1.0f, 1.0f, 1.0f},
         .intensity = 1.0f,
         .position = {0.f, 0.f, 0.f},
         .direction = {0.f, 0.f, -1.f}},
    };

    // --- Shaders ------------------------------------------------------------
    Shader vs("resources/shaders/vs_default.glsl", ShaderType::VERTEX);
    Shader fs("resources/shaders/fs_default.glsl", ShaderType::FRAGMENT);
    ShaderProgram shaderProgram(vs, fs);
    Log::info("Default shader program created.");

    // --- Renderer -----------------------------------------------------------
    Renderer renderer(scene, textureManager);

    // --- Input --------------------------------------------------------------
    InputManager inputManager(window_.handle());
    CameraController cameraController(scene.camera, inputManager);

    auto _escConn = inputManager.key(GLFW_KEY_ESCAPE).pressed.connect([&]
                                                                      { ::glfwSetWindowShouldClose(window_.handle(), true); });

    // --- Main loop ----------------------------------------------------------
    Log::info("Starting main loop.");
    while (!window_.shouldClose())
    {
        timer_.update();
        inputManager.update();
        cameraController.update(timer_.deltaTime());

        // Keep the flashlight attached to the camera
        auto &flashlight = scene.lights.back();
        flashlight.position = scene.camera.position;
        flashlight.direction = scene.camera.direction;

        // Render
        renderer.beginFrame();
        for (auto &entity : scene.entities)
            renderer.submit(entity, shaderProgram);
        renderer.endFrame();

        window_.swapBuffers();
        Window::pollEvents();
    }
}
