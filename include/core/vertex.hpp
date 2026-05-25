#pragma once

#include <glm/glm.hpp>

// Structure to standardize the vertices used in the meshes
struct Vertex
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(0.0f);
    glm::vec3 tangent = glm::vec3(0.0f);
    glm::vec3 bitangent = glm::vec3(0.0f);
    glm::vec2 texCoords = glm::vec2(0.0f);
};