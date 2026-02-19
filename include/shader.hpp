#pragma once

#include <format>
#include <string_view>

#include <glad/glad.h>

#include "autoRelease.hpp"

enum class ShaderType
{
    VERTEX,
    FRAGMENT
};

class Shader
{
public:
    Shader(std::string_view sourcePath, ShaderType type);
    ShaderType type() const;
    ::GLuint nativeHandle() const;

private:
    void checkCompileErrors() const;

    AutoRelease<::GLuint> handle_;
    ShaderType type_;
};

template <>
struct std::formatter<ShaderType>
{
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const ShaderType &type, std::format_context &ctx) const
    {
        switch (type)
        {
            using enum ShaderType;
        case VERTEX:
            return std::format_to(ctx.out(), "VERTEX");
        case FRAGMENT:
            return std::format_to(ctx.out(), "FRAGMENT");
        }
        throw std::format_error("invalid shader type"); // TODO : my own error type ?
    }
};