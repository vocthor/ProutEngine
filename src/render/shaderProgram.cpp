#include "render/shaderProgram.hpp"

namespace
{
    void checkState(::GLuint handle, ::GLenum state, std::string_view msg)
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
::GLint ShaderProgram::getUniformLocation(const std::string &name) const
{
    assert(("ShaderProgram must be bound before setting uniforms", bound_));
    auto it = uniformCache_.find(name);
    if (it != uniformCache_.end())
        return it->second;
    ::GLint loc = ::glGetUniformLocation(handle_, name.c_str());
    uniformCache_.emplace(name, loc);
    return loc;
}

void ShaderProgram::bindUniformBlock(const std::string &blockName, ::GLuint bindingPoint) const
{
    const ::GLuint blockIndex = ::glGetUniformBlockIndex(handle_, blockName.c_str());
    if (blockIndex != GL_INVALID_INDEX)
        ::glUniformBlockBinding(handle_, blockIndex, bindingPoint);
}

void ShaderProgram::checkCompileErrors() const
{
    checkState(handle_, GL_LINK_STATUS, "Failed to link shader program");

    ::glValidateProgram(handle_);
    checkState(handle_, GL_VALIDATE_STATUS, "Failed to validate shader program");
}
