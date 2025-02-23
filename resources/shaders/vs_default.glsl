#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

out vec2 ourTextureCoord;
out vec3 ourNormal;
out vec3 fragPos;

void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));
    ourTextureCoord = aTextureCoord;
    // ! matrix inversion should be calculated on the CPU, and not the GPU (just like the model matrix)
    ourNormal = normal * aNormal;

    // note that we read the multiplication from right to left
    gl_Position = projection * view * vec4(fragPos, 1.0);
};