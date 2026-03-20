#include "render/VBO.hpp"

// Constructor that generates a Vertex Buffer Object and links it to vertices
VBO::VBO(std::vector<Vertex> &vertices)
    : handle_{0z, [](::GLuint h)
              { ::glDeleteBuffers(1, &h); }}
{
    ::glGenBuffers(1, &handle_);
    ::glBindBuffer(GL_ARRAY_BUFFER, handle_);
    ::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

// Binds the VBO
void VBO::bind()
{
    ::glBindBuffer(GL_ARRAY_BUFFER, handle_);
}

// Unbinds the VBO
void VBO::unbind()
{
    ::glBindBuffer(GL_ARRAY_BUFFER, 0);
}