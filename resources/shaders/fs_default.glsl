#version 330 core

in vec4 ourColor;
in vec2 ourTextureCoord;
in vec3 ourNormal;
in vec3 fragPos;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec4 lightColor;
uniform vec3 viewPos;

out vec4 FragColor;

void main()
{
    // Ambiant lighting
    float ambiant = 0.2f;

    // Diffuse lighting
    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diffuse = max(dot(norm, lightDir), 0.0);

    // Specular lighting
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float specular = spec * specularStrength;

    // Textures
    vec4 res_textures = mix(texture(texture0, ourTextureCoord), texture(texture1, ourTextureCoord), 0.5);

    FragColor =  res_textures * ourColor * lightColor * (ambiant + diffuse + specular);
};