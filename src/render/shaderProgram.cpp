#include "render/shaderProgram.hpp"

namespace
{
    void checkState(::GLuint handle, GLenum state, std::string_view msg)
    {
        ::GLint success;
        ::glGetProgramiv(handle, state, &success);
        if (!success)
        {
            char infoLog[1024];
            ::glGetProgramInfoLog(handle, sizeof(infoLog), nullptr, infoLog);
            throw std::runtime_error(
                std::format("ProgramShader creation error {} : {}", msg, infoLog));
        }
    }
}

ShaderProgram::ShaderProgram(const Shader &vertexShader, const Shader &fragmentShader)
    : handle_{}
{
    assert(("Shader is not a vertex shader", vertexShader.type() == ShaderType::VERTEX));
    assert(("Shader is not a fragment shader", fragmentShader.type() == ShaderType::FRAGMENT));

    // shader Program
    handle_ = AutoRelease<::GLuint>{::glCreateProgram(), ::glDeleteProgram};
    if (!handle_)
    {
        throw std::runtime_error("Failed to create shader program");
    }
    // ::glObjectLabel(GL_PROGRAM, handle_, name.length(), name.data()); // TODO : OpenGL 4.3+ only

    ::glAttachShader(handle_, vertexShader.nativeHandle());
    ::glAttachShader(handle_, fragmentShader.nativeHandle());
    ::glLinkProgram(handle_);
    checkCompileErrors();
}

void ShaderProgram::use()
{
    // assert(("ShaderProgram is already bound", !bound_));
    ::glUseProgram(handle_);
    bound_ = true;
}

// ! TODO : ca risque pas de péter si on bind un autre shader entre temps ?
::GLint ShaderProgram::uniformLocation(const std::string &name) const
{
    assert(("ShaderProgram must be bound before setting uniforms", bound_));
    auto it = uniformCache_.find(name);
    if (it != uniformCache_.end())
        return it->second;
    ::GLint loc = ::glGetUniformLocation(handle_, name.c_str());
    uniformCache_.emplace(name, loc);
    return loc;
}

void ShaderProgram::setBool(const std::string &name, bool value) const
{
    ::glUniform1i(uniformLocation(name), (int)value);
}

void ShaderProgram::setInt(const std::string &name, int value) const
{
    ::glUniform1i(uniformLocation(name), value);
}

void ShaderProgram::setFloat(const std::string &name, float value) const
{
    ::glUniform1f(uniformLocation(name), value);
}

void ShaderProgram::setVec2(const std::string &name, const glm::vec2 &value) const
{
    ::glUniform2fv(uniformLocation(name), 1, &value[0]);
}

void ShaderProgram::setVec2(const std::string &name, float x, float y) const
{
    ::glUniform2f(uniformLocation(name), x, y);
}

void ShaderProgram::setVec3(const std::string &name, const glm::vec3 &value) const
{
    ::glUniform3fv(uniformLocation(name), 1, &value[0]);
}

void ShaderProgram::setVec3(const std::string &name, float x, float y, float z) const
{
    ::glUniform3f(uniformLocation(name), x, y, z);
}

void ShaderProgram::setVec4(const std::string &name, const glm::vec4 &value) const
{
    ::glUniform4fv(uniformLocation(name), 1, &value[0]);
}

void ShaderProgram::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    ::glUniform4f(uniformLocation(name), x, y, z, w);
}

void ShaderProgram::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    ::glUniformMatrix2fv(uniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    ::glUniformMatrix3fv(uniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    ::glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void ShaderProgram::checkCompileErrors() const
{
    checkState(handle_, GL_LINK_STATUS, "Failed to link shader program");

    ::glValidateProgram(handle_);
    checkState(handle_, GL_VALIDATE_STATUS, "Failed to validate shader program");
}
