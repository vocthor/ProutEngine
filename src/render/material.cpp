#include "render/material.hpp"

#include <glad/glad.h>

Material::Material(std::shared_ptr<ShaderProgram> shader)
    : shader_{std::move(shader)}
{
}

void Material::setTexture(std::string_view slot, std::shared_ptr<Texture> texture)
{
    textures_[std::string(slot)] = std::move(texture);
}

void Material::bind() const
{
    shader_->use();

    int unit = 0;
    for (const auto &[slot, texture] : textures_)
    {
        texture->bind(unit);
        shader_->setInt(slot, unit);
        ++unit;
    }

    for (const auto &[name, value] : properties_)
    {
        std::visit([&](const auto &v)
                   {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, float>)
                shader_->setFloat(name, v);
            else if constexpr (std::is_same_v<T, int>)
                shader_->setInt(name, v);
            else if constexpr (std::is_same_v<T, glm::vec3>)
                shader_->setVec3(name, v);
            else if constexpr (std::is_same_v<T, glm::vec4>)
                shader_->setVec4(name, v);
            else if constexpr (std::is_same_v<T, glm::mat4>)
                shader_->setMat4(name, v); }, value);
    }
}

void Material::unbind() const
{
    int unit = 0;
    for (const auto &[slot, texture] : textures_)
    {
        ::glActiveTexture(GL_TEXTURE0 + unit);
        texture->unbind();
        ++unit;
    }
}

ShaderProgram &Material::shader() const
{
    return *shader_;
}
