#include "game/gameLayer.hpp"

#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "core/timer.hpp"
#include "core/window.hpp"
#include "input/inputManager.hpp"
#include "modelBuilder.hpp"
#include "render/material.hpp"
#include "render/renderTarget.hpp"
#include "render/renderer.hpp"
#include "render/shader.hpp"
#include "scene/light.hpp"
#include "scene/scene.hpp"
#include "textureManager.hpp"
#include "utils/log.hpp"

// ---------------------------------------------------------------------------
void GameLayer::onAttach()
{
    TextureManager &textureManager = ctx().textureManager;
    Scene &scene = ctx().scene;

    // --- Resources ----------------------------------------------------------
    backpackModel_ = std::make_unique<Model>(
        ModelBuilder::load("resources/models/backpack/backpack.obj", textureManager).build());

    // Cube — single shared geometry; all instances point to the same Model
    Material cubeMat{
        .albedoMap = textureManager.load("resources/textures/container2.png"),
        .metallicMap = textureManager.load("resources/textures/container2_specular.png"),
        .roughness = 0.4f,
    };
    cubeModel_ = std::make_unique<Model>(ModelBuilder::cube(std::move(cubeMat)).build());

    // --- Scene --------------------------------------------------------------
    scene.entities.push_back(Entity{.name = "Backpack", .model = backpackModel_.get()});

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
        Entity cube{.name = "Cube_" + std::to_string(i), .model = cubeModel_.get()};
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
    shaderProgram_ = std::make_unique<ShaderProgram>(vs, fs);
    Log::info("Default shader program created.");

    // --- Input --------------------------------------------------------------
    cameraController_ = std::make_unique<CameraController>(scene.camera, ctx().inputManager);

    GLFWwindow *window = ctx().window.handle();
    escConn_ = ctx().inputManager.key(GLFW_KEY_ESCAPE).pressed.connect([window]
                                                                       { ::glfwSetWindowShouldClose(window, true); });
}

void GameLayer::onDetach()
{
    escConn_.disconnect();
    cameraController_.reset();
    ctx().scene.entities.clear();
}

// ---------------------------------------------------------------------------
void GameLayer::onUpdate()
{
    Scene &scene = ctx().scene;

    if (ctx().gameInputEnabled)
        cameraController_->update(ctx().timer.deltaTime());
    else
        ctx().inputManager.setCursorMode(GLFW_CURSOR_NORMAL);

    // Keep the flashlight attached to the camera
    auto &flashlight = scene.lights.back();
    flashlight.position = scene.camera.position;
    flashlight.direction = scene.camera.direction;
}

void GameLayer::onRender()
{
    Scene &scene = ctx().scene;
    RenderTarget &target = *ctx().renderTarget;

    target.bind();
    scene.camera.width = static_cast<int>(target.width());
    scene.camera.height = static_cast<int>(target.height());

    ctx().renderer.beginFrame();
    for (auto &entity : scene.entities)
        ctx().renderer.submit(entity, *shaderProgram_);
    ctx().renderer.endFrame();
}
