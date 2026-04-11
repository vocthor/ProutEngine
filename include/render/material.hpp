#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

#include <glm/glm.hpp>

#include "render/shaderProgram.hpp"
#include "render/texture.hpp"

using UniformValue = std::variant<float, int, glm::vec3, glm::vec4, glm::mat4>;

// TODO : Revoir structure une fois au PBR : faire une struct avec les champs roughness, metallic, ... comme Unreal
class Material
{
public:
    explicit Material(std::shared_ptr<ShaderProgram> shader);

    /**
     * @brief Sets a texture to the material.
     *
     * The slot is the name of the uniform in the shader that will be used to bind the texture.
     *
     * @param slot The name of the uniform in the shader that will be used to bind the texture. Example: "diffuse", "specular", "normal", etc.
     * @param texture The texture to set. It will be bound to the slot when the material is bound.
     */
    void setTexture(std::string_view slot, std::shared_ptr<Texture> texture);

    /**
     * @brief Sets a property to the material.
     *
     * The name is the name of the uniform in the shader that will be used to set the property.
     *
     * @tparam T The type of the property value. Example: float, int, glm::vec3, glm::vec4, glm::mat4, etc.
     * @param name The name of the uniform in the shader.
     * @param value The value to set for the property.
     */
    template <typename T>
    void setProperty(std::string_view name, T value)
    {
        properties_[std::string(name)] = UniformValue{value};
    }

    /**
     * @brief Binds the material, setting all textures and properties to the shader.
     *
     * This function will activate the shader associated with the material and bind all textures
     * and properties to their respective uniforms in the shader.
     */
    void bind() const;
    void unbind() const;

    ShaderProgram &shader() const;

private:
    std::shared_ptr<ShaderProgram> shader_;
    std::unordered_map<std::string, std::shared_ptr<Texture>> textures_;
    std::unordered_map<std::string, UniformValue> properties_;
};