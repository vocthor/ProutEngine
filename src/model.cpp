#include "model.hpp"

#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "utils/log.hpp"

Model::Model(std::string_view path, TextureManager &textureManager)
    : textureManager_{textureManager}
{
    loadModel(path);
}

void Model::draw(ShaderProgram &shaderProgram, Camera &camera)
{
    // Bind shaderProgram to be able to access uniforms
    shaderProgram.use();
    // Take care of the camera Matrix
    shaderProgram.setVec3("viewPos", camera.position);
    camera.matrix(50.0f, 0.1f, 100.0f, shaderProgram);

    for (auto &mesh : meshes_)
    {
        mesh.material.bind(shaderProgram, textureManager_);
        mesh.draw();
    }
}

void Model::loadModel(std::string_view path)
{
    // read file via ASSIMP
    ::Assimp::Importer importer;
    const ::aiScene *scene = importer.ReadFile(path.data(), ::aiProcess_Triangulate | ::aiProcess_FlipUVs | ::aiProcess_CalcTangentSpace); // aiProcess_GenNormals | aiProcess_SplitLargeMeshes | aiProcess_OptimizeMeshes
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Log::error("ASSIMP: {}", importer.GetErrorString());
        return;
    }
    // retrieve the directory path of the filepath
    directory_ = path.substr(0, path.find_last_of('/'));
    Log::info("Loading model from {}", directory_);
    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

void Model::processNode(::aiNode *node, const ::aiScene *scene)
{
    Log::debug("Processing node {}", node->mName.C_Str());
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        ::aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes_.push_back(processMesh(mesh, scene));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(::aiMesh *mesh, const ::aiScene *scene)
{
    Log::debug("Processing mesh {}", mesh->mName.C_Str());
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    /***** Vertices *****/
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // tangent & bitangent
        if (mesh->HasTangentsAndBitangents())
        {
            vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        }
        // texture coordinates
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        }
        else
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }

    /***** Indices *****/
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        ::aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    /***** Material *****/
    ::aiMaterial *aiMat = scene->mMaterials[mesh->mMaterialIndex];

    // Albedo (diffuse)
    material.albedoMap = loadTexture(aiMat, ::aiTextureType_DIFFUSE);
    // Normal map (Assimp maps bump to HEIGHT for OBJ)
    material.normalMap = loadTexture(aiMat, ::aiTextureType_HEIGHT);
    if (material.normalMap == TextureHandle::Invalid)
        material.normalMap = loadTexture(aiMat, ::aiTextureType_NORMALS);
    // Metallic (specular map as approximation)
    material.metallicMap = loadTexture(aiMat, ::aiTextureType_SPECULAR);
    // Roughness
    material.roughnessMap = loadTexture(aiMat, ::aiTextureType_DIFFUSE_ROUGHNESS);
    if (material.roughnessMap == TextureHandle::Invalid)
        material.roughnessMap = tryLoadFile("roughness.jpg");
    // AO
    material.aoMap = loadTexture(aiMat, ::aiTextureType_AMBIENT_OCCLUSION);
    if (material.aoMap == TextureHandle::Invalid)
        material.aoMap = loadTexture(aiMat, ::aiTextureType_LIGHTMAP);
    if (material.aoMap == TextureHandle::Invalid)
        material.aoMap = tryLoadFile("ao.jpg");

    return Mesh(vertices, indices, material);
}

TextureHandle Model::loadTexture(::aiMaterial *mat, ::aiTextureType type)
{
    if (mat->GetTextureCount(type) == 0)
        return TextureHandle::Invalid;
    ::aiString str;
    mat->GetTexture(type, 0, &str);
    std::string path = directory_ + "/" + str.C_Str();
    Log::debug("Loading texture: {}", path);
    return textureManager_.load(path);
}

TextureHandle Model::tryLoadFile(const std::string &filename)
{
    std::string path = directory_ + "/" + filename;
    std::ifstream f(path);
    if (!f.good())
        return TextureHandle::Invalid;
    Log::debug("Loading texture (heuristic): {}", path);
    return textureManager_.load(path);
}