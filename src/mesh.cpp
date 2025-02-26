#include "mesh.hpp"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    vao.bind();
    // Generates Vertex Buffer Object and links it to vertices
    VBO VBO(vertices);
    // Generates Element Buffer Object and links it to indices
    EBO EBO(indices);
    // Links VBO attributes such as coordinates and colors to VAO
    vao.linkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void *)0);                           // Position
    vao.linkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, normal));    // Normal
    vao.linkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, color));     // Color
    vao.linkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void *)offsetof(Vertex, texCoords)); // Texture coords
    // Unbind all to prevent accidentally modifying them
    vao.unbind();
    VBO.unbind();
    EBO.unbind();
}

void Mesh::draw(Shader &shader, Camera &camera)
{
    // Bind shader to be able to access uniforms
    shader.use();
    vao.bind();

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

        textures[i].texUnit(shader, ("material." + type + num).c_str(), i);
        textures[i].bind();
    }
    // Take care of the camera Matrix
    shader.setVec3("viewPos", camera.position);
    camera.matrix(50.0f, 0.1f, 100.0f, shader);

    // Draw the actual mesh
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao.unbind();
}

void Mesh::remove()
{
    // ! TODO : VBO and EBO as attributes ? !
    vao.remove();
    for (Texture &t : textures)
    {
        t.remove();
    }
}
