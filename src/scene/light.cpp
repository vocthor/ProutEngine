#include "scene/light.hpp"

#include <format>

#include <glm/trigonometric.hpp>

#include "render/shaderProgram.hpp"

namespace LightUtils
{
    void uploadToShader(ShaderProgram &shader, const DirectionalLight &light)
    {
        shader.setVec3("dirLight.direction", light.direction);
        shader.setVec3("dirLight.ambient", light.ambient);
        shader.setVec3("dirLight.diffuse", light.diffuse);
        shader.setVec3("dirLight.specular", light.specular);
    }

    void uploadToShader(ShaderProgram &shader, const PointLight &light, int index)
    {
        const std::string base = std::format("pointLights[{}].", index);
        shader.setVec3(base + "position", light.position);
        shader.setVec3(base + "ambient", light.ambient);
        shader.setVec3(base + "diffuse", light.diffuse);
        shader.setVec3(base + "specular", light.specular);
        shader.setFloat(base + "constant", light.constant);
        shader.setFloat(base + "linear", light.linear);
        shader.setFloat(base + "quadratic", light.quadratic);
    }

    void uploadToShader(ShaderProgram &shader, const SpotLight &light)
    {
        shader.setVec3("spotLight.position", light.position);
        shader.setVec3("spotLight.direction", light.direction);
        shader.setVec3("spotLight.ambient", light.ambient);
        shader.setVec3("spotLight.diffuse", light.diffuse);
        shader.setVec3("spotLight.specular", light.specular);
        shader.setFloat("spotLight.constant", light.constant);
        shader.setFloat("spotLight.linear", light.linear);
        shader.setFloat("spotLight.quadratic", light.quadratic);
        // Shader expects cosines, we store degrees for ergonomics
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(light.cutOff)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(light.outerCutOff)));
    }
} // namespace LightUtils
