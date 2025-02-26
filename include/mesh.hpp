#ifndef MESH_H
#define MESH_H

#include "VAO.hpp"
#include "EBO.hpp"
#include "camera.hpp"
#include "texture.hpp"

class Mesh
{
public:
    // mesh data
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void draw(Shader &shader, Camera &camera);
    void remove();

private:
    VAO vao;
};

#endif
