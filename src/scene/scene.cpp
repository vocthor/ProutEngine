#include "scene/scene.hpp"

#include "render/shaderProgram.hpp"

void Scene::uploadLights(ShaderProgram &shader) const
{
    shader.setVec3("ambientColor", ambientColor);
    LightUtils::uploadToShader(shader, lights);
}
