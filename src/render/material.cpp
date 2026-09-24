#include "render/material.hpp"

#include <glad/glad.h>

#include "render/shaderProgram.hpp"

void Material::bind(ShaderProgram &shader, TextureManager &texMgr) const
{
    auto bindSlot = [&](int unit, const char *sampler, const char *hasFlag, TextureHandle h)
    {
        bool has = (h != TextureHandle::Invalid);
        shader.setUniform<bool>(hasFlag, has);
        if (has)
        {
            texMgr.get(h).bind(unit);
            shader.setUniform<int>(sampler, unit);
        }
    };

    bindSlot(0, "material.albedoMap", "material.hasAlbedoMap", albedoMap);
    bindSlot(1, "material.normalMap", "material.hasNormalMap", normalMap);
    bindSlot(2, "material.metallicMap", "material.hasMetallicMap", metallicMap);
    bindSlot(3, "material.roughnessMap", "material.hasRoughnessMap", roughnessMap);
    bindSlot(4, "material.aoMap", "material.hasAoMap", aoMap);

    shader.setUniform<glm::vec3>("material.albedo", albedo);
    shader.setUniform<float>("material.metallic", metallic);
    shader.setUniform<float>("material.roughness", roughness);
    shader.setUniform<float>("material.ao", ao);
}
