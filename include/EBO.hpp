#pragma once

#include <glad/glad.h>
#include <vector>

#include "autoRelease.hpp"

class EBO
{
public:
    AutoRelease<::GLuint> handle_;
    // Constructor that generates a Elements Buffer Object and links it to indices
    EBO(std::vector<::GLuint> &indices);

    // Binds the EBO
    void bind();
    // Unbinds the EBO
    void unbind();
};