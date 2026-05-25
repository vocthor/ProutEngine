#pragma once

#include "render/VAO.hpp"
#include "render/gpuBuffer.hpp"
#include "render/material.hpp"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<::GLuint> indices;
    Material material;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, Material material);
    void draw();

private:
    VAO vao_;
    VBO vbo_;
    EBO ebo_;
};
