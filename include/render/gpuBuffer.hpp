#pragma once

#include <glad/glad.h>
#include <span>

#include "core/vertex.hpp"
#include "utils/autoRelease.hpp"

template <::GLenum Target, typename DataType>
class GPUBuffer
{
public:
    GPUBuffer(std::span<const DataType> data, ::GLenum usage = GL_STATIC_DRAW)
        : handle_{0z, [](::GLuint h)
                  { ::glDeleteBuffers(1, &h); }}
    {
        ::glGenBuffers(1, &handle_);
        ::glBindBuffer(Target, handle_);
        ::glBufferData(Target, data.size() * sizeof(DataType), data.data(), usage);
    }

    // Allocates an uninitialized buffer of the given byte size (e.g. for UBOs updated later via glBufferSubData)
    explicit GPUBuffer(::GLsizeiptr sizeBytes, ::GLenum usage = GL_DYNAMIC_DRAW)
        : handle_{0z, [](::GLuint h)
                  { ::glDeleteBuffers(1, &h); }}
    {
        ::glGenBuffers(1, &handle_);
        ::glBindBuffer(Target, handle_);
        ::glBufferData(Target, sizeBytes, nullptr, usage);
    }

    void bind() const
    {
        ::glBindBuffer(Target, handle_);
    }

    void unbind() const
    {
        ::glBindBuffer(Target, 0);
    }

    ::GLuint handle() const
    {
        return handle_;
    }

private:
    AutoRelease<::GLuint> handle_;
};

using VBO = GPUBuffer<GL_ARRAY_BUFFER, Vertex>;
using EBO = GPUBuffer<GL_ELEMENT_ARRAY_BUFFER, ::GLuint>;
using UBO = GPUBuffer<GL_UNIFORM_BUFFER, std::byte>;