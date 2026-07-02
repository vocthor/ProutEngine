#include "core/application.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "cameraController.hpp"
#include "debug/imguiLayer.hpp"
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
    ImGuiLayer imguiLayer(window_.handle());

    // --- Resources ----------------------------------------------------------
    TextureManager textureManager;

    Model backpackModel =
        ModelBuilder::load("resources/models/backpack/backpack.obj", textureManager).build();

    // Cube — single shared geometry; all instances point to the same Model
    Material cubeMat{
        .albedoMap = textureManager.load("resources/textures/container2.png"),
        .metallicMap = textureManager.load("resources/textures/container2_specular.png"),
        .roughness = 0.4f,
    };
    Model cubeModel = ModelBuilder::cube(std::move(cubeMat)).build();

    // --- Scene --------------------------------------------------------------
    Scene scene(Camera(static_cast<int>(window_.width()),
                       static_cast<int>(window_.height()),
                       glm::vec3(0.0f, 0.0f, 2.0f)));

    scene.entities.push_back(Entity{.name = "Backpack", .model = &backpackModel});

    // 10 cubes at the positions from the original tutorial
    constexpr glm::vec3 cubePositions[] = {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 5.0f, -15.0f},
        {-1.5f, -2.2f, -2.5f},
        {-3.8f, -2.0f, -12.3f},
        {2.4f, -0.4f, -3.5f},
        {-1.7f, 3.0f, -7.5f},
        {1.3f, -2.0f, -2.5f},
        {1.5f, 2.0f, -2.5f},
        {1.5f, 0.2f, -1.5f},
        {-1.3f, 1.0f, -1.5f},
    };
    for (int i = 0; i < 10; ++i)
    {
        Entity cube{.name = "Cube_" + std::to_string(i), .model = &cubeModel};
        cube.transform.position = cubePositions[i];
        cube.transform.rotation = glm::vec3(20.0f * static_cast<float>(i), 0.0f, 0.0f);
        scene.entities.push_back(std::move(cube));
    }

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

        imguiLayer.beginFrame();

        if (!imguiLayer.wantsMouseCapture() && !imguiLayer.wantsKeyboardCapture())
        {
            cameraController.update(timer_.deltaTime());
        }
        else
        {
            inputManager.setCursorMode(GLFW_CURSOR_NORMAL);
        }

        // Keep the flashlight attached to the camera
        auto &flashlight = scene.lights.back();
        flashlight.position = scene.camera.position;
        flashlight.direction = scene.camera.direction;

        imguiLayer.renderDebugPanel(scene);

        // Render
        renderer.beginFrame();
        for (auto &entity : scene.entities)
            renderer.submit(entity, shaderProgram);
        renderer.endFrame();

        imguiLayer.endFrame();

        window_.swapBuffers();
        Window::pollEvents();
    }
}
