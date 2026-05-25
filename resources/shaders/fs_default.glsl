#version 410 core

// ---------------------------------------------------------------------------
// PBR Cook-Torrance BRDF — GGX distribution, Smith geometry, Fresnel-Schlick.
// Uses unified lights[] array from step 4.2.
// ---------------------------------------------------------------------------

#define MAX_LIGHTS 16

const int LIGHT_DIRECTIONAL = 0;
const int LIGHT_POINT       = 1;
const int LIGHT_SPOT        = 2;

const float PI = 3.14159265359;

struct Light {
    int   type;
    vec3  color;
    float intensity;
    vec3  position;
    vec3  direction;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

struct Material {
    sampler2D albedoMap;
    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
    sampler2D aoMap;

    bool hasAlbedoMap;
    bool hasNormalMap;
    bool hasMetallicMap;
    bool hasRoughnessMap;
    bool hasAoMap;

    vec3  albedo;
    float metallic;
    float roughness;
    float ao;
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
// PBR functions
// ---------------------------------------------------------------------------

// Normal Distribution Function — GGX/Trowbridge-Reitz
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a  = roughness * roughness;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return a2 / max(denom, 0.0001);
}

// Geometry function — Schlick-GGX (single direction)
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

// Geometry function — Smith (both directions)
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

// Fresnel — Schlick approximation
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// ---------------------------------------------------------------------------
// Per-light radiance computation
// ---------------------------------------------------------------------------
vec3 CalcLight(Light light, vec3 N, vec3 V, vec3 fragPos,
               vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3  L;
    float attenuation   = 1.0;
    float spotIntensity = 1.0;

    if (light.type == LIGHT_DIRECTIONAL)
    {
        L = normalize(-light.direction);
    }
    else
    {
        L = normalize(light.position - fragPos);
        float dist = length(light.position - fragPos);
        attenuation = 1.0 / (light.constant + light.linear * dist
                             + light.quadratic * dist * dist);

        if (light.type == LIGHT_SPOT)
        {
            float theta   = dot(L, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            spotIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        }
    }

    vec3 H = normalize(V + L);
    vec3 radiance = light.color * light.intensity * attenuation * spotIntensity;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G   = GeometrySmith(N, V, L, roughness);
    vec3  F   = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3  numerator   = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3  specular    = numerator / denominator;

    // Energy conservation
    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

// ---------------------------------------------------------------------------
void main()
{
    // Sample material properties
    vec3 albedo = material.hasAlbedoMap
        ? pow(texture(material.albedoMap, TextureCoord).rgb, vec3(2.2))
        : material.albedo;

    float metallic = material.hasMetallicMap
        ? texture(material.metallicMap, TextureCoord).r
        : material.metallic;

    float roughness = material.hasRoughnessMap
        ? texture(material.roughnessMap, TextureCoord).r
        : material.roughness;

    float ao = material.hasAoMap
        ? texture(material.aoMap, TextureCoord).r
        : material.ao;

    // Normal
    vec3 N;
    if (material.hasNormalMap)
    {
        N = texture(material.normalMap, TextureCoord).rgb;
        N = N * 2.0 - 1.0;
        N = normalize(TBN * N);
    }
    else
    {
        N = normalize(Normal);
    }

    vec3 V = normalize(viewPos - FragPos);

    // F0 — base reflectivity (dielectric ≈ 0.04, metallic = albedo)
    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    // Accumulate lighting
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numLights; i++)
        Lo += CalcLight(lights[i], N, V, FragPos, albedo, metallic, roughness, F0);

    // Ambient (simple constant, IBL would go here later)
    vec3 ambient = ambientColor * albedo * ao;
    vec3 color = ambient + Lo;

    // Reinhard tone mapping
    color = color / (color + vec3(1.0));
    // Gamma correction (linear → sRGB)
    color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}