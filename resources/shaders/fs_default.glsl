#version 330 core


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;  

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
    // Ambient lighting
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse lighting
    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    // Textures
    vec4 res_textures = mix(texture(texture0, ourTextureCoord), texture(texture1, ourTextureCoord), 0.5);

    FragColor =  res_textures * ourColor * lightColor * vec4(ambient + diffuse + specular, 1.0);
};