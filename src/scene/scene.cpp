#include "scene/scene.hpp"

#include "render/shaderProgram.hpp"

void Scene::uploadLights(ShaderProgram &shader) const
{
    LightUtils::uploadToShader(shader, directionalLight);

    for (int i = 0; i < static_cast<int>(pointLights.size()); ++i)
        LightUtils::uploadToShader(shader, pointLights[i], i);

    LightUtils::uploadToShader(shader, spotLight);
}
