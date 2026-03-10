#pragma once

#include <string>

#include <glad/glad.h>

#include "autoRelease.hpp"

class Texture
{
public:
    Texture(const std::string &path, ::GLenum pixelType = GL_UNSIGNED_BYTE);

    void bind(::GLuint unit) const;
    void unbind() const;

    ::GLuint handle() const { return handle_.get(); }
    const std::string &path() const { return path_; }

private:
    AutoRelease<::GLuint> handle_;
    std::string path_;
    ::GLenum format_;
};