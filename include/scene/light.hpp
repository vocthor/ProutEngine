#pragma once

#include <glm/glm.hpp>
#include <span>

class ShaderProgram;

// -----------------------------------------------------------------------------
// Unified light model — color × intensity instead of ambient/diffuse/specular.
// Prepares the transition to PBR (6.11): the shader receives radiance directly.
// -----------------------------------------------------------------------------

inline constexpr int MAX_LIGHTS = 16;

enum class LightType : int
{
    DIRECTIONAL = 0,
    POINT = 1,
    SPOT = 2
};

struct Light
{
    LightType type = LightType::POINT;

    glm::vec3 color = glm::vec3(1.0f);
    float intensity = 1.0f;

    // Position (POINT/SPOT) — ignored for DIRECTIONAL
    glm::vec3 position = glm::vec3(0.0f);
    // Direction (DIRECTIONAL/SPOT) — ignored for POINT
    glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f);

    // Attenuation (POINT/SPOT only)
    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

    // SPOT cone half-angles in degrees (converted to cosines on upload)
    float cutOff = 12.5f;
    float outerCutOff = 17.5f;
};

// -----------------------------------------------------------------------------
// LightUtils — upload lights to shader uniforms.
// The shader must already be active (use() called beforehand).
// -----------------------------------------------------------------------------
namespace LightUtils
{
    void uploadToShader(ShaderProgram &shader, std::span<const Light> lights);
} // namespace LightUtils
