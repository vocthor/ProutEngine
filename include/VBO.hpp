#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// Structure to standardize the vertices used in the meshes
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoords;
};

class VBO
{
public:
    // Reference ID of the Vertex Buffer Object
    GLuint ID;
    // Constructor that generates a Vertex Buffer Object and links it to vertices
    VBO(std::vector<Vertex> &vertices);

    // Binds the VBO
    void bind();
    // Unbinds the VBO
    void unbind();
    // Deletes the VBO
    void remove();
};

#endif