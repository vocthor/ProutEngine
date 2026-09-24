#pragma once

#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "utils/autoRelease.hpp"
#include "render/shader.hpp"

class ShaderProgram
{
public:
    ShaderProgram(const Shader &vertexShader, const Shader &fragmentShader);

    void use();

    template <typename T>
    void setUniform(const std::string &name, const T &value) const
    {
        if constexpr (std::is_same_v<T, bool>)
            ::glUniform1i(getUniformLocation(name), (int)value);
        else if constexpr (std::is_same_v<T, int>)
            ::glUniform1i(getUniformLocation(name), value);
        else if constexpr (std::is_same_v<T, float>)
            ::glUniform1f(getUniformLocation(name), value);
        else if constexpr (std::is_same_v<T, glm::vec2>)
            ::glUniform2fv(getUniformLocation(name), 1, &value[0]);
        else if constexpr (std::is_same_v<T, glm::vec3>)
            ::glUniform3fv(getUniformLocation(name), 1, &value[0]);
        else if constexpr (std::is_same_v<T, glm::vec4>)
            ::glUniform4fv(getUniformLocation(name), 1, &value[0]);
        else if constexpr (std::is_same_v<T, glm::mat2>)
            ::glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
        else if constexpr (std::is_same_v<T, glm::mat3>)
            ::glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
        else if constexpr (std::is_same_v<T, glm::mat4>)
            ::glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
    }

    void bindUniformBlock(const std::string &blockName, ::GLuint bindingPoint) const;

private:
    ::GLint getUniformLocation(const std::string &name) const;
    void checkCompileErrors() const;

    AutoRelease<::GLuint> handle_;
    bool bound_ = false;

    mutable std::unordered_map<std::string, ::GLint> uniformCache_;
};
