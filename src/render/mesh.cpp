#include "render/mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    : vertices{std::move(vertices)},
      indices{std::move(indices)},
      vbo_{this->vertices}, // ! Mind the move() on the parameter before
      ebo_{this->indices}   // ! Mind the move() on the parameter before
{
    vao_.bind();
    ebo_.bind();
    // Links VBO attributes such as coordinates and colors to VAO
    vao_.linkAttrib(vbo_, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);                           // Position
    vao_.linkAttrib(vbo_, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));    // Normal
    vao_.linkAttrib(vbo_, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, color));     // Color
    vao_.linkAttrib(vbo_, 4, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, tangent));   // Tangent
    vao_.linkAttrib(vbo_, 5, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, bitangent)); // Bitangent
    vao_.linkAttrib(vbo_, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, texCoords)); // Texture coords
    // Unbind all to prevent accidentally modifying them
    vao_.unbind();
    vbo_.unbind();
    ebo_.unbind();
}

void Mesh::draw()
{
    vao_.bind();
    ::glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao_.unbind();
}
