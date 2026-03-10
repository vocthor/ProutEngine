#pragma once

#include "VAO.hpp"
#include "EBO.hpp"
#include "camera.hpp"
#include "textureManager.hpp"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<::GLuint> indices;
    std::vector<TextureRef> texturesRefs;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TextureRef> texturesRefs);
    void draw(ShaderProgram &shaderProgram, Camera &camera, TextureManager &textureManager);

private:
    VAO vao_;
    VBO vbo_;
    EBO ebo_;
};
