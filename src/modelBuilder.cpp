#include "modelBuilder.hpp"

#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "utils/log.hpp"

// ---------------------------------------------------------------------------
// Assimp helpers — confined to this translation unit
// ---------------------------------------------------------------------------
namespace
{

    TextureHandle loadTexture(::aiMaterial *mat, ::aiTextureType type,
                              TextureManager &texMgr, const std::string &dir)
    {
        if (mat->GetTextureCount(type) == 0)
            return TextureHandle::Invalid;
        ::aiString str;
        mat->GetTexture(type, 0, &str);
        std::string path = dir + "/" + str.C_Str();
        Log::debug("Loading texture: {}", path);
        return texMgr.load(path);
    }

    TextureHandle tryLoadFile(const std::string &filename,
                              TextureManager &texMgr, const std::string &dir)
    {
        std::string path = dir + "/" + filename;
        std::ifstream f(path);
        if (!f.good())
            return TextureHandle::Invalid;
        Log::debug("Loading texture (heuristic): {}", path);
        return texMgr.load(path);
    }

    ModelPart processAssimpMesh(::aiMesh *mesh, const ::aiScene *scene,
                                TextureManager &texMgr, const std::string &dir)
    {
        Log::debug("Processing mesh {}", mesh->mName.C_Str());
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        Material material;

        /***** Vertices *****/
        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.position = vector;
            if (mesh->HasNormals())
            {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.normal = vector;
            }
            if (mesh->HasTangentsAndBitangents())
            {
                vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
            }
            if (mesh->mTextureCoords[0])
            {
                vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x,
                                             mesh->mTextureCoords[0][i].y);
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

        material.albedoMap = loadTexture(aiMat, ::aiTextureType_DIFFUSE, texMgr, dir);
        material.normalMap = loadTexture(aiMat, ::aiTextureType_HEIGHT, texMgr, dir);
        if (material.normalMap == TextureHandle::Invalid)
            material.normalMap = loadTexture(aiMat, ::aiTextureType_NORMALS, texMgr, dir);
        material.metallicMap = loadTexture(aiMat, ::aiTextureType_SPECULAR, texMgr, dir);
        material.roughnessMap = loadTexture(aiMat, ::aiTextureType_DIFFUSE_ROUGHNESS, texMgr, dir);
        if (material.roughnessMap == TextureHandle::Invalid)
            material.roughnessMap = tryLoadFile("roughness.jpg", texMgr, dir);
        material.aoMap = loadTexture(aiMat, ::aiTextureType_AMBIENT_OCCLUSION, texMgr, dir);
        if (material.aoMap == TextureHandle::Invalid)
            material.aoMap = loadTexture(aiMat, ::aiTextureType_LIGHTMAP, texMgr, dir);
        if (material.aoMap == TextureHandle::Invalid)
            material.aoMap = tryLoadFile("ao.jpg", texMgr, dir);

        return ModelPart{Mesh(std::move(vertices), std::move(indices)), std::move(material)};
    }

    void processNode(::aiNode *node, const ::aiScene *scene,
                     std::vector<ModelPart> &parts,
                     TextureManager &texMgr, const std::string &dir)
    {
        Log::debug("Processing node {}", node->mName.C_Str());
        // process all the node's meshes (if any)
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
            parts.push_back(processAssimpMesh(scene->mMeshes[node->mMeshes[i]], scene, texMgr, dir));
        // then do the same for each of its children
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            processNode(node->mChildren[i], scene, parts, texMgr, dir);
    }

} // namespace

// ---------------------------------------------------------------------------
// ModelBuilder
// ---------------------------------------------------------------------------

ModelBuilder ModelBuilder::load(std::string_view path, TextureManager &textureManager)
{
    ModelBuilder builder;
    // read file via ASSIMP
    ::Assimp::Importer importer;
    const ::aiScene *scene = importer.ReadFile(
        path.data(),
        ::aiProcess_Triangulate | ::aiProcess_FlipUVs | ::aiProcess_CalcTangentSpace);

    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Log::error("ASSIMP: {}", importer.GetErrorString());
        return builder;
    }
    // retrieve the directory path of the filepath
    std::string dir(path.substr(0, path.find_last_of('/')));
    Log::info("Loading model from {}", dir);
    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene, builder.parts_, textureManager, dir);
    return builder;
}

ModelBuilder ModelBuilder::procedural()
{
    return ModelBuilder{};
}

ModelBuilder ModelBuilder::cube(Material material)
{
    // clang-format off
    // 24 vertices (4 per face) — normals, tangents and bitangents are exact for
    // each face so that normal-mapping works correctly out of the box.
    std::vector<Vertex> verts = {
        // Front  (N= 0, 0,+1 | T=+1, 0, 0 | B= 0,+1, 0)
        {{-0.5f,-0.5f, 0.5f},{0,0,1},{},{1,0,0},{0,1,0},{0,0}},
        {{-0.5f, 0.5f, 0.5f},{0,0,1},{},{1,0,0},{0,1,0},{0,1}},
        {{ 0.5f, 0.5f, 0.5f},{0,0,1},{},{1,0,0},{0,1,0},{1,1}},
        {{ 0.5f,-0.5f, 0.5f},{0,0,1},{},{1,0,0},{0,1,0},{1,0}},
        // Back   (N= 0, 0,-1 | T=-1, 0, 0 | B= 0,+1, 0)
        {{-0.5f,-0.5f,-0.5f},{0,0,-1},{},{-1,0,0},{0,1,0},{1,0}},
        {{-0.5f, 0.5f,-0.5f},{0,0,-1},{},{-1,0,0},{0,1,0},{1,1}},
        {{ 0.5f, 0.5f,-0.5f},{0,0,-1},{},{-1,0,0},{0,1,0},{0,1}},
        {{ 0.5f,-0.5f,-0.5f},{0,0,-1},{},{-1,0,0},{0,1,0},{0,0}},
        // Bottom (N= 0,-1, 0 | T=+1, 0, 0 | B= 0, 0,-1)
        {{-0.5f,-0.5f, 0.5f},{0,-1,0},{},{1,0,0},{0,0,-1},{0,0}},
        {{ 0.5f,-0.5f, 0.5f},{0,-1,0},{},{1,0,0},{0,0,-1},{1,0}},
        {{ 0.5f,-0.5f,-0.5f},{0,-1,0},{},{1,0,0},{0,0,-1},{1,1}},
        {{-0.5f,-0.5f,-0.5f},{0,-1,0},{},{1,0,0},{0,0,-1},{0,1}},
        // Top    (N= 0,+1, 0 | T=+1, 0, 0 | B= 0, 0,+1)
        {{-0.5f, 0.5f, 0.5f},{0,1,0},{},{1,0,0},{0,0,1},{0,1}},
        {{ 0.5f, 0.5f, 0.5f},{0,1,0},{},{1,0,0},{0,0,1},{1,1}},
        {{ 0.5f, 0.5f,-0.5f},{0,1,0},{},{1,0,0},{0,0,1},{1,0}},
        {{-0.5f, 0.5f,-0.5f},{0,1,0},{},{1,0,0},{0,0,1},{0,0}},
        // Right  (N=+1, 0, 0 | T= 0, 0,+1 | B= 0,-1, 0)
        {{ 0.5f, 0.5f, 0.5f},{1,0,0},{},{0,0,1},{0,-1,0},{1,1}},
        {{ 0.5f,-0.5f, 0.5f},{1,0,0},{},{0,0,1},{0,-1,0},{1,0}},
        {{ 0.5f,-0.5f,-0.5f},{1,0,0},{},{0,0,1},{0,-1,0},{0,0}},
        {{ 0.5f, 0.5f,-0.5f},{1,0,0},{},{0,0,1},{0,-1,0},{0,1}},
        // Left   (N=-1, 0, 0 | T= 0, 0,-1 | B= 0,+1, 0)
        {{-0.5f,-0.5f, 0.5f},{-1,0,0},{},{0,0,-1},{0,1,0},{0,0}},
        {{-0.5f, 0.5f, 0.5f},{-1,0,0},{},{0,0,-1},{0,1,0},{0,1}},
        {{-0.5f, 0.5f,-0.5f},{-1,0,0},{},{0,0,-1},{0,1,0},{1,1}},
        {{-0.5f,-0.5f,-0.5f},{-1,0,0},{},{0,0,-1},{0,1,0},{1,0}},
    };
    std::vector<unsigned int> idx = {
         0, 1, 2,  0, 2, 3,  // Front
         4, 5, 6,  4, 6, 7,  // Back
         8, 9,10,  8,10,11,  // Bottom
        12,13,14, 12,14,15,  // Top
        16,17,18, 16,18,19,  // Right
        20,21,22, 20,22,23,  // Left
    };
    // clang-format on
    ModelBuilder builder = ModelBuilder::procedural();
    builder.addPart(std::move(verts), std::move(idx), std::move(material));
    return builder;
}

ModelBuilder &ModelBuilder::addPart(std::vector<Vertex> vertices,
                                    std::vector<unsigned int> indices,
                                    Material material)
{
    parts_.push_back(ModelPart{
        Mesh(std::move(vertices), std::move(indices)),
        std::move(material)});
    return *this;
}

Model ModelBuilder::build()
{
    return Model(std::move(parts_));
}
