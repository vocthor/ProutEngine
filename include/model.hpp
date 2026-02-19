#pragma once

#include <assimp/scene.h>

#include "mesh.hpp"

class Model
{
public:
    Model(std::string_view path);
    void draw(ShaderProgram &shaderProgram, Camera &camera);

private:
    // model data
    std::vector<Mesh> meshes_;
    std::string directory_;
    std::vector<Texture> loadedTextures_;

    void loadModel(std::string_view path);
    void processNode(::aiNode *node, const ::aiScene *scene);
    Mesh processMesh(::aiMesh *mesh, const ::aiScene *scene);
    std::vector<Texture> loadMaterialTextures(::aiMaterial *mat, ::aiTextureType type, std::string typeName);
};
