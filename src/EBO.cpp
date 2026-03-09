#include "EBO.hpp"

// Constructor that generates a Elements Buffer Object and links it to indices
EBO::EBO(std::vector<::GLuint> &indices)
    : handle_{0z, [](::GLuint h)
              { ::glDeleteBuffers(1, &h); }}
{
    ::glGenBuffers(1, &handle_);
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle_);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(::GLuint), indices.data(), GL_STATIC_DRAW);
}

// Binds the EBO
void EBO::bind()
{
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle_);
}

// Unbinds the EBO
void EBO::unbind()
{
    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}