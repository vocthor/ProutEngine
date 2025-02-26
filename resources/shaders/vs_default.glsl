#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTextureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out vec2 TextureCoord;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // ! matrix inversion should be calculated on the CPU, and not the GPU (just like the model matrix)
    Normal = normal * aNormal;
    TextureCoord = aTextureCoord;
    Color = aColor;

    // note that we read the multiplication from right to left
    gl_Position = projection * view * vec4(FragPos, 1.0);
};