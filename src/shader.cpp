#include "shader.hpp"

#include <fstream>
#include <sstream>

namespace
{
    ::GLenum toNative(ShaderType type)
    {
        switch (type)
        {
            using enum ShaderType;
        case VERTEX:
            return GL_VERTEX_SHADER;
        case FRAGMENT:
            return GL_FRAGMENT_SHADER;
        }
        throw std::invalid_argument("invalid shader type");
    }

    std::string loadFromFile(std::string_view path)
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            file.open(path.data());
            std::stringstream contentStream;
            contentStream << file.rdbuf();
            file.close();
            return contentStream.str();
        }
        catch (const std::exception &e)
        {
            // TODO : my own error type ?
            throw std::runtime_error(std::string("failed to load shader source from file: ") + e.what());
        }
    }
}

Shader::Shader(std::string_view sourcePath, ShaderType type)
    : handle_{}, type_(type)
{
    handle_ = AutoRelease<::GLuint>{::glCreateShader(toNative(type)), ::glDeleteShader};
    // ::glObjectLabel(GL_SHADER, handle_, name.length(), name.data()); // TODO : OpenGL 4.3+ only

    const std::string source = loadFromFile(sourcePath);
    const ::GLchar *strings[] = {source.data()};
    const ::GLint lengths[] = {static_cast<::GLint>(source.length())};

    ::glShaderSource(handle_, 1, strings, lengths);
    ::glCompileShader(handle_);

    checkCompileErrors();
}

ShaderType Shader::type() const
{
    return type_;
}

::GLuint Shader::nativeHandle() const
{
    return handle_;
}

void Shader::checkCompileErrors() const
{
    ::GLint success;
    ::glGetShaderiv(handle_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        ::glGetShaderInfoLog(handle_, sizeof(infoLog), nullptr, infoLog);
        // TODO : my own error type ?
        throw std::runtime_error(
            std::format("Failed to compile shader of type {} : {}", type_, infoLog));
    }
}
