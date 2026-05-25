#version 410 core

// ---------------------------------------------------------------------------
// Unified light model — color × intensity, Blinn-Phong shading.
// Prepares the transition to PBR Cook-Torrance (Phase 6.11).
// ---------------------------------------------------------------------------

#define MAX_LIGHTS 16

const int LIGHT_DIRECTIONAL = 0;
const int LIGHT_POINT       = 1;
const int LIGHT_SPOT        = 2;

struct Light {
    int   type;
    vec3  color;
    float intensity;
    vec3  position;
    vec3  direction;
    float constant;
    float linear;
    float quadratic;
    float cutOff;       // cosine of inner cone angle
    float outerCutOff;  // cosine of outer cone angle
};

struct Material {
    sampler2D texture_diffuse0;
    sampler2D texture_specular0;
    sampler2D texture_normal0;
    float shininess;
    bool hasNormalMap;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TextureCoord;
in mat3 TBN;

uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform Material material;
uniform Light lights[MAX_LIGHTS];
uniform int  numLights;

out vec4 FragColor;

// ---------------------------------------------------------------------------
// Single light contribution (Blinn-Phong).
// albedo = diffuse texture sample, spec = specular texture sample.
// ---------------------------------------------------------------------------
vec3 CalcLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir,
               vec3 albedo, vec3 spec)
{
    vec3  lightDir;
    float attenuation    = 1.0;
    float spotIntensity  = 1.0;

    if (light.type == LIGHT_DIRECTIONAL)
    {
        lightDir = normalize(-light.direction);
    }
    else
    {
        lightDir = normalize(light.position - fragPos);
        float dist = length(light.position - fragPos);
        attenuation = 1.0 / (light.constant + light.linear * dist
                             + light.quadratic * dist * dist);

        if (light.type == LIGHT_SPOT)
        {
            float theta   = dot(lightDir, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            spotIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        }
    }

    // Blinn-Phong (half-vector based — closer to PBR specular lobe)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3  halfDir = normalize(lightDir + viewDir);
    float specFactor = pow(max(dot(normal, halfDir), 0.0), material.shininess);

    // Radiance from this light (color × intensity × falloff)
    vec3 radiance = light.color * light.intensity * attenuation * spotIntensity;

    vec3 diffuse  = diff * albedo * radiance;
    vec3 specular = specFactor * spec * radiance;

    return diffuse + specular;
}

void main()
{
    vec3 norm;
    if (material.hasNormalMap)
    {
        // Sample normal map (tangent space) and transform to world space
        norm = texture(material.texture_normal0, TextureCoord).rgb;
        norm = norm * 2.0 - 1.0; // [0,1] → [-1,1]
        norm = normalize(TBN * norm);
    }
    else
    {
        norm = normalize(Normal);
    }

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 albedo = pow(vec3(texture(material.texture_diffuse0, TextureCoord)), vec3(2.2));
    vec3 spec   = pow(vec3(texture(material.texture_specular0, TextureCoord)), vec3(2.2));

    // Global ambient
    vec3 result = ambientColor * albedo;

    // Accumulate all lights
    for (int i = 0; i < numLights; i++)
        result += CalcLight(lights[i], norm, FragPos, viewDir, albedo, spec);

    // Reinhard tone mapping
    result = result / (result + vec3(1.0));
    // Gamma correction (linear → sRGB)
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}