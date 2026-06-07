#pragma once

#include <vector>

#include "camera.hpp"
#include "render/mesh.hpp"
#include "render/material.hpp"
#include "render/shaderProgram.hpp"

// One rendered unit: pure geometry paired with its appearance.
struct ModelPart
{
    Mesh mesh;
    Material material;
};

class Model
{
public:
    void draw(ShaderProgram &shaderProgram, Camera &camera, TextureManager &textureManager);

    const std::vector<ModelPart> &parts() const { return parts_; }

private:
    explicit Model(std::vector<ModelPart> parts);
    friend class ModelBuilder;

    std::vector<ModelPart> parts_;
};
