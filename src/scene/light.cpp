#include "scene/light.hpp"

#include <format>

#include <glm/trigonometric.hpp>

#include "render/shaderProgram.hpp"

namespace LightUtils
{
    void uploadToShader(ShaderProgram &shader, std::span<const Light> lights)
    {
        shader.setUniform<int>("numLights", lights.size());

        for (size_t i = 0; i < lights.size(); ++i)
        {
            const auto &l = lights[i];
            const std::string base = std::format("lights[{}].", i);

            shader.setUniform<int>(base + "type", static_cast<int>(l.type));
            shader.setUniform<glm::vec3>(base + "color", l.color);
            shader.setUniform<float>(base + "intensity", l.intensity);
            shader.setUniform<glm::vec3>(base + "position", l.position);
            shader.setUniform<glm::vec3>(base + "direction", l.direction);
            shader.setUniform<float>(base + "constant", l.constant);
            shader.setUniform<float>(base + "linear", l.linear);
            shader.setUniform<float>(base + "quadratic", l.quadratic);
            shader.setUniform<float>(base + "cutOff", glm::cos(glm::radians(l.cutOff)));
            shader.setUniform<float>(base + "outerCutOff", glm::cos(glm::radians(l.outerCutOff)));
        }
    }
} // namespace LightUtils
