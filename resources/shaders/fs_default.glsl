#version 330 core

struct Material {
    sampler2D diffuse; // now diffuse = ambient
    sampler2D specular;
    float shininess;
}; 

struct Light {
    vec3 position; 
    vec3 direction; 
    float cutOff;
    float outerCutOff;
    
    vec3 ambient; // aka lightColor
    vec3 diffuse;
    vec3 specular;

    // Distance attenuation varaibles
    float constant;
    float linear;
    float quadratic;
};

in vec2 ourTextureCoord;
in vec3 ourNormal;
in vec3 fragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;  

out vec4 FragColor;

void main()
{
    // Ambient lighting
    vec3 ambient = light.ambient * texture(material.diffuse, ourTextureCoord).rgb;

    // Diffuse lighting
    vec3 norm = normalize(ourNormal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, ourTextureCoord).rgb;

    // Specular lighting
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular,ourTextureCoord).rgb;

    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // Distance attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    ambient *= attenuation;  
    diffuse *= attenuation;
    specular *= attenuation;   

    FragColor = vec4(ambient + diffuse + specular, 1.0);
};