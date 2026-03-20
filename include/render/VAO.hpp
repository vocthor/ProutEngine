#pragma once

#include <glad/glad.h>
#include "utils/autoRelease.hpp"
#include "render/VBO.hpp"

class VAO
{
public:
    AutoRelease<::GLuint> handle_;
    // Constructor that generates a VAO ID
    VAO();

    // Links a VBO Attribute such as a position or color to the VAO
    void linkAttrib(VBO &VBO, ::GLuint layout, ::GLuint numComponents, ::GLenum type, ::GLsizeiptr stride, void *offset);
    // Binds the VAO
    void bind();
    // Unbinds the VAO
    void unbind();
};