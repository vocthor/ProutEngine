#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "render/cameraUBO.hpp"

// Forward declarations — full types included in renderer.cpp
class Mesh;
struct Material;
class ShaderProgram;
struct Entity;
class Scene;
class TextureManager;

// ---------------------------------------------------------------------------
// RenderCommand — one draw call: geometry + appearance + per-object transform
// ---------------------------------------------------------------------------
struct RenderCommand
{
    Mesh *mesh = nullptr;
    Material *material = nullptr;
    ShaderProgram *shader = nullptr;
    glm::mat4 modelMatrix{1.0f};
    glm::mat3 normalMatrix{1.0f};
};

// ---------------------------------------------------------------------------
// Renderer — collects RenderCommands, sorts them (by shader then material) to
// minimise GPU state switches, then issues all draw calls in flush().
//
// Usage per frame:
//   renderer.beginFrame();
//   renderer.submit(entity, shader);        // or renderer.submit(cmd) manually
//   renderer.endFrame();                    // sorts, draws, clears queue
//   window.swapBuffers();
// ---------------------------------------------------------------------------
class Renderer
{
public:
    Renderer(Scene &scene, TextureManager &texMgr);

    // glClear at the start of a frame.
    void beginFrame();

    // Enqueue a single pre-built draw command.
    void submit(RenderCommand cmd);

    // Decompose an Entity (Model → [Mesh × Material]) into RenderCommands and
    // enqueue them all with the given shader.
    void submit(const Entity &entity, ShaderProgram &shader);

    // Sort the queue (shader → material), issue all draw calls, clear the queue.
    void flush();

    // Convenience: calls flush().  Window::swapBuffers() is the caller's responsibility.
    void endFrame();

private:
    // Upload scene lights and bind camera UBO for this shader.
    void uploadLightsAndBindCameraUBO(ShaderProgram &shader);

    Scene &scene_;
    TextureManager &texMgr_;
    CameraUBO cameraUBO_;

    std::vector<RenderCommand> renderQueue_;
};
