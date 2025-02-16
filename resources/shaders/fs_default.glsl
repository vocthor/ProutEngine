#version 330 core

in vec4 ourColor;
in vec2 ourTextureCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 FragColor;

void main()
{
    FragColor = mix(texture(texture0, ourTextureCoord), texture(texture1, ourTextureCoord), 0.5) * ourColor;
};