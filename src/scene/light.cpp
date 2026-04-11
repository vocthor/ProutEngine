#include "scene/light.hpp"

#include <format>

#include <glm/trigonometric.hpp>

#include "render/shaderProgram.hpp"

namespace LightUtils
{
    void uploadToShader(ShaderProgram &shader, std::span<const Light> lights)
    {
        shader.setInt("numLights", lights.size());

        for (size_t i = 0; i < lights.size(); ++i)
        {
            const auto &l = lights[i];
            const std::string base = std::format("lights[{}].", i);

            shader.setInt(base + "type", static_cast<int>(l.type));
            shader.setVec3(base + "color", l.color);
            shader.setFloat(base + "intensity", l.intensity);
            shader.setVec3(base + "position", l.position);
            shader.setVec3(base + "direction", l.direction);
            shader.setFloat(base + "constant", l.constant);
            shader.setFloat(base + "linear", l.linear);
            shader.setFloat(base + "quadratic", l.quadratic);
            shader.setFloat(base + "cutOff", glm::cos(glm::radians(l.cutOff)));
            shader.setFloat(base + "outerCutOff", glm::cos(glm::radians(l.outerCutOff)));
        }
    }
} // namespace LightUtils
