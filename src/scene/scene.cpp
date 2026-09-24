#include "scene/scene.hpp"

#include "render/shaderProgram.hpp"

void Scene::uploadLights(ShaderProgram &shader) const
{
    shader.setUniform<glm::vec3>("ambientColor", ambientColor);
    LightUtils::uploadToShader(shader, lights);
}
