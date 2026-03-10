#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    : vertices{vertices}, indices{indices}, textures{textures},
      vbo_{vertices}, ebo_{indices}
{
    vao_.bind();
    ebo_.bind();
    // Links VBO attributes such as coordinates and colors to VAO
    vao_.linkAttrib(vbo_, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);                           // Position
    vao_.linkAttrib(vbo_, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));    // Normal
    vao_.linkAttrib(vbo_, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, color));     // Color
    vao_.linkAttrib(vbo_, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, texCoords)); // Texture coords
    // Unbind all to prevent accidentally modifying them
    vao_.unbind();
    vbo_.unbind();
    ebo_.unbind();
}

void Mesh::draw(ShaderProgram &shaderProgram, Camera &camera)
{
    // Bind shaderProgram to be able to access uniforms
    shaderProgram.use();
    vao_.bind();

    // Keep track of how many of each type of textures we have
    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        std::string num;
        std::string type = textures[i].type;

        if (type == "texture_diffuse")
            num = std::to_string(numDiffuse++);
        else if (type == "texture_specular")
            num = std::to_string(numSpecular++);

        textures[i].texUnit(shaderProgram, ("material." + type + num).c_str(), i);
        textures[i].bind();
    }
    // Take care of the camera Matrix
    shaderProgram.setVec3("viewPos", camera.position);
    camera.matrix(50.0f, 0.1f, 100.0f, shaderProgram);

    // Draw the actual mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao_.unbind();
}
