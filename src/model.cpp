#include "model.hpp"

Model::Model(std::vector<ModelPart> parts)
    : parts_{std::move(parts)}
{}

void Model::draw(ShaderProgram &shaderProgram, Camera &camera, TextureManager &textureManager)
{
    // Bind shaderProgram to be able to access uniforms
    shaderProgram.use();
    // Take care of the camera Matrix
    shaderProgram.setVec3("viewPos", camera.position);
    camera.matrix(50.0f, 0.1f, 100.0f, shaderProgram);

    for (auto &part : parts_)
    {
        part.material.bind(shaderProgram, textureManager);
        part.mesh.draw();
    }
}
