#pragma once

#include "render/VAO.hpp"
#include "render/gpuBuffer.hpp"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<::GLuint> indices;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    void draw();

private:
    VAO vao_;
    VBO vbo_;
    EBO ebo_;
};
