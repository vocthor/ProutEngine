#include "render/material.hpp"

#include <glad/glad.h>

#include "render/shaderProgram.hpp"

void Material::bind(ShaderProgram &shader, TextureManager &texMgr) const
{
    auto bindSlot = [&](int unit, const char *sampler, const char *hasFlag, TextureHandle h)
    {
        bool has = (h != TextureHandle::Invalid);
        shader.setBool(hasFlag, has);
        if (has)
        {
            texMgr.get(h).bind(unit);
            shader.setInt(sampler, unit);
        }
    };

    bindSlot(0, "material.albedoMap", "material.hasAlbedoMap", albedoMap);
    bindSlot(1, "material.normalMap", "material.hasNormalMap", normalMap);
    bindSlot(2, "material.metallicMap", "material.hasMetallicMap", metallicMap);
    bindSlot(3, "material.roughnessMap", "material.hasRoughnessMap", roughnessMap);
    bindSlot(4, "material.aoMap", "material.hasAoMap", aoMap);

    shader.setVec3("material.albedo", albedo);
    shader.setFloat("material.metallic", metallic);
    shader.setFloat("material.roughness", roughness);
    shader.setFloat("material.ao", ao);
}
