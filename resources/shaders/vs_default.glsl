#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTextureCoord;
layout (location = 3) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal;

out vec4 ourColor;
out vec2 ourTextureCoord;
out vec3 ourNormal;
out vec3 fragPos;

void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));

    // note that we read the multiplication from right to left
    gl_Position = projection * view * vec4(fragPos, 1.0);
    
    ourColor = aColor;
    ourTextureCoord = aTextureCoord;
    // ! matrix inversion should be calculated on the CPU, and not the GPU (just like the model matrix)
    ourNormal = normal * aNormal;
};