#pragma once

#include <assimp/scene.h>

#include "camera.hpp"
#include "render/mesh.hpp"
#include "render/shaderProgram.hpp"

class Model
{
public:
    Model(std::string_view path, TextureManager &textureManager);
    void draw(ShaderProgram &shaderProgram, Camera &camera);

private:
    std::vector<Mesh> meshes_;
    std::string directory_;
    TextureManager &textureManager_;

    void loadModel(std::string_view path);
    void processNode(::aiNode *node, const ::aiScene *scene);
    Mesh processMesh(::aiMesh *mesh, const ::aiScene *scene);
    TextureHandle loadTexture(::aiMaterial *mat, ::aiTextureType type);
    TextureHandle tryLoadFile(const std::string &filename);
};
