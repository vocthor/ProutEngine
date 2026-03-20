#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "utils/autoRelease.hpp"

// Structure to standardize the vertices used in the meshes
// TODO : define my own Vector3, ... data structures
struct Vertex
{
    glm::vec3 position = glm::vec3(0.0f);  // default value to prevent uninitialized data
    glm::vec3 normal = glm::vec3(0.0f);    // default value to prevent uninitialized data
    glm::vec3 color = glm::vec3(0.0f);     // default value to prevent uninitialized data
    glm::vec2 texCoords = glm::vec2(0.0f); // default value to prevent uninitialized data
};

class VBO
{
public:
    AutoRelease<::GLuint> handle_;
    // Constructor that generates a Vertex Buffer Object and links it to vertices
    VBO(std::vector<Vertex> &vertices);

    // Binds the VBO
    void bind();
    // Unbinds the VBO
    void unbind();
};
