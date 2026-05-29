#pragma once

#include <string_view>
#include <vector>

#include "model.hpp"

class ModelBuilder
{
public:
    // Load all parts from a 3D asset file (OBJ, FBX, …) via Assimp.
    static ModelBuilder load(std::string_view path, TextureManager &textureManager);

    // Start an empty builder for procedural geometry.
    static ModelBuilder procedural();

    // Append one geometry + material pair (chainable).
    ModelBuilder &addPart(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material);

    // Finalize and return the Model.
    Model build();

private:
    ModelBuilder() = default;

    std::vector<ModelPart> parts_;
};
