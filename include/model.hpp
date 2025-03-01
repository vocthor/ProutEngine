#ifndef MODEL_H
#define MODEL_H

#include <assimp/scene.h>

#include "mesh.hpp"

class Model
{
public:
    Model(char *path);
    void draw(Shader &shader, Camera &camera);

private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif // MODEL_H