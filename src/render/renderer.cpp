#include "render/renderer.hpp"

#include <algorithm>

#include <glad/glad.h>

#include "model.hpp"
#include "render/mesh.hpp"
#include "render/material.hpp"
#include "render/shaderProgram.hpp"
#include "scene/entity.hpp"
#include "scene/scene.hpp"
#include "textureManager.hpp"

Renderer::Renderer(Scene &scene, TextureManager &texMgr)
    : scene_{scene}, texMgr_{texMgr}
{
}

void Renderer::beginFrame()
{
    ::glClearColor(scene_.ambientColor.r, scene_.ambientColor.g, scene_.ambientColor.b, scene_.ambientColor.a);
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Renderer::submit(RenderCommand cmd)
{
    renderQueue_.push_back(std::move(cmd));
}

void Renderer::submit(const Entity &entity, ShaderProgram &shader)
{
    if (!entity.model)
        return;

    const glm::mat4 modelMatrix = entity.transform.getModelMatrix();
    const glm::mat3 normalMatrix = entity.transform.getNormalMatrix();

    for (ModelPart &part : entity.model->parts())
    {
        submit(RenderCommand{
            .mesh = &part.mesh,
            .material = &part.material,
            .shader = &shader,
            .modelMatrix = modelMatrix,
            .normalMatrix = normalMatrix,
        });
    }
}

void Renderer::flush()
{
    // Sort by shader pointer first, then by material pointer.
    // This minimises GPU state switches (shader changes are the most expensive).
    std::stable_sort(renderQueue_.begin(), renderQueue_.end(),
                     [](const RenderCommand &a, const RenderCommand &b)
                     {
                         if (a.shader != b.shader)
                             return a.shader < b.shader;
                         return a.material < b.material;
                     });

    ShaderProgram *activeShader   = nullptr;
    Material      *activeMaterial = nullptr;

    for (const RenderCommand &cmd : renderQueue_)
    {
        if (!cmd.mesh || !cmd.shader)
            continue;

        // Shader switch: re-upload camera matrices and scene lights once per shader.
        if (cmd.shader != activeShader)
        {
            activeShader = cmd.shader;
            activeMaterial = nullptr; // force material rebind after a shader switch
            activeShader->use();
            uploadCameraAndLights(*activeShader);
        }

        // Material switch: rebind textures and material uniforms.
        if (cmd.material != activeMaterial)
        {
            activeMaterial = cmd.material;
            if (activeMaterial)
                activeMaterial->bind(*activeShader, texMgr_);
        }

        // Per-object uniforms.
        activeShader->setMat4("model", cmd.modelMatrix);
        activeShader->setMat3("normal", cmd.normalMatrix);

        cmd.mesh->draw();
    }

    renderQueue_.clear();
}

void Renderer::endFrame()
{
    flush();
}

void Renderer::uploadCameraAndLights(ShaderProgram &shader)
{
    shader.setVec3("viewPos", scene_.camera.position);
    scene_.camera.matrix(50.0f, 0.1f, 100.0f, shader);
    scene_.uploadLights(shader);
}
