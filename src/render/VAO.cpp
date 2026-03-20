#include "render/VAO.hpp"

// Constructor that generates a VAO ID
VAO::VAO()
    : handle_{0z, [](::GLuint h)
              { ::glDeleteVertexArrays(1, &h); }}
{
    ::glGenVertexArrays(1, &handle_);
}

// Links a VBO Attribute such as a position or color to the VAO
void VAO::linkAttrib(VBO &VBO, ::GLuint layout, ::GLuint numComponents, ::GLenum type, ::GLsizeiptr stride, void *offset)
{
    VBO.bind();
    ::glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
    ::glEnableVertexAttribArray(layout);
    VBO.unbind();
}

// Binds the VAO
void VAO::bind()
{
    ::glBindVertexArray(handle_);
}

// Unbinds the VAO
void VAO::unbind()
{
    ::glBindVertexArray(0);
}