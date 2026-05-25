#pragma once

#include <glm/glm.hpp>

#include "textureManager.hpp"

// PBR Material — struct-based like Unreal (albedo, metallic, roughness, ao, normal).
// Each texture slot is optional; when Invalid the shader uses the scalar fallback.
struct Material
{
    // Texture maps (TextureHandle::Invalid = not present)
    TextureHandle albedoMap = TextureHandle::Invalid;
    TextureHandle normalMap = TextureHandle::Invalid;
    TextureHandle metallicMap = TextureHandle::Invalid;
    TextureHandle roughnessMap = TextureHandle::Invalid;
    TextureHandle aoMap = TextureHandle::Invalid;

    // Scalar fallbacks (used when corresponding map is missing)
    glm::vec3 albedo = glm::vec3(0.5f);
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;

    // Bind all textures and scalars to the shader at fixed units
    void bind(class ShaderProgram &shader, TextureManager &texMgr) const;
};