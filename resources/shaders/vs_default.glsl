#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

out vec2 TextureCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    TextureCoord = aTextureCoord;
    // ! matrix inversion should be calculated on the CPU, and not the GPU (just like the model matrix)
    Normal = normal * aNormal;

    // note that we read the multiplication from right to left
    gl_Position = projection * view * vec4(FragPos, 1.0);
};