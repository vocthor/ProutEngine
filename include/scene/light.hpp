#pragma once

#include <variant>

#include <glm/glm.hpp>

class ShaderProgram; // forward declaration — avoid pulling in heavy headers

// -----------------------------------------------------------------------------
// Light structs — fields match the GLSL uniforms in fs_default.glsl exactly.
// Ambient/diffuse/specular follow the Phong shading model.
// -----------------------------------------------------------------------------

struct DirectionalLight
{
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight
{
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    // Attenuation coefficients (see https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation)
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
};

struct SpotLight
{
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    float cutOff = 12.5f;      // inner cone half-angle, in degrees
    float outerCutOff = 17.5f; // outer cone half-angle, in degrees
};

using Light = std::variant<DirectionalLight, PointLight, SpotLight>;

// -----------------------------------------------------------------------------
// LightUtils — upload a light struct to a ShaderProgram's uniforms.
// The shader is expected to already be active (use() called beforehand).
// cutOff/outerCutOff are converted to cosines automatically.
// -----------------------------------------------------------------------------
namespace LightUtils
{
    void uploadToShader(ShaderProgram &shader, const DirectionalLight &light);

    // index corresponds to pointLights[index] in the shader
    void uploadToShader(ShaderProgram &shader, const PointLight &light, int index);

    void uploadToShader(ShaderProgram &shader, const SpotLight &light);
} // namespace LightUtils
