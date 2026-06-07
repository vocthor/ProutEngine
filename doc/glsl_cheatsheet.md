# GLSL Cheatsheet — OpenGL 3.3 → 4.1

Référence rapide pour les shaders GLSL utilisés dans ProutEngine.

---

## Table des matières

- [GLSL Cheatsheet — OpenGL 3.3 → 4.1](#glsl-cheatsheet--opengl-33--41)
  - [Table des matières](#table-des-matières)
  - [1. Versions et `#version`](#1-versions-et-version)
  - [2. Types de shaders](#2-types-de-shaders)
  - [3. Types de données](#3-types-de-données)
    - [Scalaires](#scalaires)
    - [Vecteurs](#vecteurs)
    - [Matrices](#matrices)
  - [4. Qualifiers (stockage)](#4-qualifiers-stockage)
  - [5. Layout qualifiers](#5-layout-qualifiers)
    - [Locations d'attributs](#locations-dattributs)
    - [Sorties multiples (MRT — Multiple Render Targets)](#sorties-multiples-mrt--multiple-render-targets)
    - [UBO binding](#ubo-binding)
  - [6. Uniforms et blocs](#6-uniforms-et-blocs)
    - [Uniforms simples](#uniforms-simples)
    - [Tableaux d'uniforms](#tableaux-duniforms)
    - [Uniform Buffer Objects (UBO) — voir section 11](#uniform-buffer-objects-ubo--voir-section-11)
  - [7. Fonctions built-in essentielles](#7-fonctions-built-in-essentielles)
    - [Mathématiques](#mathématiques)
    - [Trigonométrie](#trigonométrie)
    - [Vecteurs](#vecteurs-1)
    - [Matrices](#matrices-1)
  - [8. Structures](#8-structures)
  - [9. Préprocesseur](#9-préprocesseur)
  - [10. Textures et samplers](#10-textures-et-samplers)
    - [Types de samplers](#types-de-samplers)
    - [Lecture d'une texture](#lecture-dune-texture)
    - [Unités de textures](#unités-de-textures)
  - [11. Uniform Buffer Objects (UBO)](#11-uniform-buffer-objects-ubo)
    - [GLSL](#glsl)
    - [Layout `std140` — règles d'alignement](#layout-std140--règles-dalignement)
    - [C++](#c)
  - [12. Interface blocks](#12-interface-blocks)
  - [13. Variables built-in](#13-variables-built-in)
    - [Vertex shader](#vertex-shader)
    - [Fragment shader](#fragment-shader)
  - [14. Swizzling](#14-swizzling)
  - [15. Précision et conversions](#15-précision-et-conversions)
    - [Conversions implicites](#conversions-implicites)
    - [Conversions explicites](#conversions-explicites)
    - [Pas de cast implicite `int → float` dans les opérations](#pas-de-cast-implicite-int--float-dans-les-opérations)
  - [16. Bonnes pratiques](#16-bonnes-pratiques)
    - [Performance](#performance)
    - [Qualité](#qualité)
    - [Organisation](#organisation)
  - [17. Différences 3.3 → 4.1](#17-différences-33--41)
  - [Annexe — Template PBR minimal](#annexe--template-pbr-minimal)

---

## 1. Versions et `#version`

La première ligne d'un shader **doit** être la directive de version :

```glsl
#version 330 core   // OpenGL 3.3, profil core (pas de fonctions dépréciées)
#version 410 core   // OpenGL 4.1, profil core
```

| OpenGL | GLSL | Nouveautés clés                                  |
| ------ | ---- | ------------------------------------------------ |
| 3.3    | 330  | Base solide : in/out, UBO, layout, instanciation |
| 4.0    | 400  | Tessellation, `double`, `subroutine`             |
| 4.1    | 410  | `glProgramUniform*` (DSA partiel), précision ES  |

Le profil `core` interdit les fonctions dépréciées (`gl_FragColor`, `varying`, `attribute`...).

---

## 2. Types de shaders

| Stage             | Fichier typique | Rôle                                          |
| ----------------- | --------------- | --------------------------------------------- |
| **Vertex**        | `vs_*.glsl`     | Transforme chaque sommet (model → clip space) |
| **Fragment**      | `fs_*.glsl`     | Calcule la couleur de chaque pixel            |
| Geometry          | `gs_*.glsl`     | Optionnel — génère/modifie des primitives     |
| Tessellation Ctrl | `tcs_*.glsl`    | Contrôle la subdivision (OpenGL 4.0+)         |
| Tessellation Eval | `tes_*.glsl`    | Évalue la surface subdivisée (OpenGL 4.0+)    |

ProutEngine utilise vertex + fragment.

---

## 3. Types de données

### Scalaires

| Type     | Description              | Exemple              |
| -------- | ------------------------ | -------------------- |
| `bool`   | Booléen                  | `bool b = true;`     |
| `int`    | Entier signé 32 bits     | `int i = 42;`        |
| `uint`   | Entier non signé 32 bits | `uint u = 3u;`       |
| `float`  | Flottant 32 bits         | `float f = 3.14;`    |
| `double` | Flottant 64 bits (4.0+)  | `double d = 3.14lf;` |

### Vecteurs

```glsl
vec2  v2 = vec2(1.0, 2.0);
vec3  v3 = vec3(1.0, 2.0, 3.0);
vec4  v4 = vec4(v3, 1.0);           // extension depuis un vec3
ivec3 iv = ivec3(1, 2, 3);          // vecteur d'entiers
bvec2 bv = bvec2(true, false);      // vecteur de booléens
```

### Matrices

```glsl
mat2  m2;       // 2×2
mat3  m3;       // 3×3 (normale matrix)
mat4  m4;       // 4×4 (model, view, projection)
mat2x3 m23;     // 2 colonnes × 3 lignes (rare)
```

Les matrices sont **column-major** en GLSL (comme GLM).

```glsl
mat4 m = mat4(1.0);     // matrice identité
vec4 col0 = m[0];       // première colonne
float elem = m[1][2];   // colonne 1, ligne 2
```

---

## 4. Qualifiers (stockage)

| Qualifier       | Direction                         | Usage                                                     |
| --------------- | --------------------------------- | --------------------------------------------------------- |
| `in`            | entrée                            | Attribut vertex (VS) ou interpolé (FS)                    |
| `out`           | sortie                            | Vers le stage suivant (VS→FS) ou framebuffer (FS)         |
| `uniform`       | CPU → GPU                         | Constante pour tous les fragments/vertices d'un draw call |
| `const`         | compile-time                      | Constante locale                                          |
| `flat`          | pas d'interpolation               | La valeur du provoking vertex est utilisée telle quelle   |
| `smooth`        | interpolation perspective-correct | Par défaut pour `out`                                     |
| `noperspective` | interpolation linéaire écran      | Utile pour certains effets 2D                             |

```glsl
// Vertex shader
in vec3 aPos;              // attribut du VBO
out vec3 FragPos;           // envoyé au fragment shader

// Fragment shader
in vec3 FragPos;            // reçu du vertex shader (interpolé)
out vec4 FragColor;         // couleur finale → framebuffer
```

---

## 5. Layout qualifiers

### Locations d'attributs

```glsl
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoord;
```

Correspond aux `glVertexAttribPointer(location, ...)` côté C++.

### Sorties multiples (MRT — Multiple Render Targets)

```glsl
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;   // pour bloom
```

### UBO binding

```glsl
layout (std140, binding = 0) uniform CameraData { ... };
```

---

## 6. Uniforms et blocs

### Uniforms simples

```glsl
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform float time;
```

Côté C++ :

```cpp
shader.setMat4("model", modelMatrix);
shader.setVec3("viewPos", camera.position);
```

### Tableaux d'uniforms

```glsl
#define MAX_LIGHTS 16
uniform int numPointLights;                       // nombre effectif
uniform PointLight pointLights[MAX_LIGHTS];       // taille fixe à la compilation
```

⚠️ Un tableau uniform a une **taille fixe**. On utilise un compteur (`numPointLights`) pour n'itérer que sur les lumières actives.

### Uniform Buffer Objects (UBO) — voir section 11

---

## 7. Fonctions built-in essentielles

### Mathématiques

| Fonction                | Description                | Exemple                         |
| ----------------------- | -------------------------- | ------------------------------- |
| `abs(x)`                | Valeur absolue             | `abs(-3.0)` → `3.0`             |
| `sign(x)`               | Signe (-1, 0, +1)          | `sign(-5.0)` → `-1.0`           |
| `floor(x)`              | Arrondi inférieur          | `floor(3.7)` → `3.0`            |
| `ceil(x)`               | Arrondi supérieur          | `ceil(3.2)` → `4.0`             |
| `fract(x)`              | Partie fractionnaire       | `fract(3.7)` → `0.7`            |
| `mod(x, y)`             | Modulo flottant            | `mod(5.0, 3.0)` → `2.0`         |
| `min(x, y)`             | Minimum                    | `min(3.0, 5.0)` → `3.0`         |
| `max(x, y)`             | Maximum                    | `max(3.0, 5.0)` → `5.0`         |
| `clamp(x, lo, hi)`      | Borne entre lo et hi       | `clamp(1.5, 0.0, 1.0)` → `1.0`  |
| `mix(a, b, t)`          | Interpolation linéaire     | `mix(0.0, 10.0, 0.3)` → `3.0`   |
| `step(edge, x)`         | 0 si x < edge, 1 sinon     | `step(0.5, 0.7)` → `1.0`        |
| `smoothstep(lo, hi, x)` | Interpolation Hermite      | Transition douce entre lo et hi |
| `pow(x, y)`             | Puissance                  | `pow(2.0, 3.0)` → `8.0`         |
| `sqrt(x)`               | Racine carrée              | `sqrt(9.0)` → `3.0`             |
| `inversesqrt(x)`        | 1 / √x (rapide)            | Normalisation optimisée         |
| `exp(x)` / `log(x)`     | Exponentielle / logarithme |                                 |

### Trigonométrie

| Fonction                             | Note               |
| ------------------------------------ | ------------------ |
| `radians(deg)`                       | Degrés → radians   |
| `degrees(rad)`                       | Radians → degrés   |
| `sin(x)` / `cos(x)` / `tan(x)`       | En **radians**     |
| `asin(x)` / `acos(x)` / `atan(y, x)` | Fonctions inverses |

### Vecteurs

| Fonction                  | Description                         |
| ------------------------- | ----------------------------------- |
| `length(v)`               | Norme euclidienne                   |
| `distance(a, b)`          | `length(a - b)`                     |
| `dot(a, b)`               | Produit scalaire                    |
| `cross(a, b)`             | Produit vectoriel (vec3 uniquement) |
| `normalize(v)`            | Vecteur unitaire                    |
| `reflect(I, N)`           | Réflexion de I par rapport à N      |
| `refract(I, N, eta)`      | Réfraction (loi de Snell)           |
| `faceforward(N, I, Nref)` | Retourne N si `dot(Nref, I) < 0`    |

### Matrices

| Fonction               | Description                              |
| ---------------------- | ---------------------------------------- |
| `transpose(m)`         | Transposée                               |
| `inverse(m)`           | Inverse (coûteux ! préférer le CPU)      |
| `determinant(m)`       | Déterminant                              |
| `matrixCompMult(a, b)` | Multiplication composante par composante |
| `outerProduct(c, r)`   | Produit extérieur                        |

---

## 8. Structures

```glsl
struct PointLight {
    vec3  position;
    vec3  color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLights[16];
```

Accès uniforms côté C++ :

```cpp
shader.setVec3("pointLights[0].position", pos);
shader.setFloat("pointLights[0].intensity", 2.5f);
```

---

## 9. Préprocesseur

```glsl
#define PI 3.14159265359
#define MAX_LIGHTS 16

#ifdef USE_NORMAL_MAP
    // code conditionnel
#endif

#ifndef SOME_FEATURE
    #define SOME_FEATURE 0
#endif
```

Utilisé côté C++ pour injecter des defines avant compilation :

```cpp
std::string src = "#define MAX_POINT_LIGHTS 4\n" + shaderSourceCode;
```

---

## 10. Textures et samplers

### Types de samplers

| Sampler           | Dimension                         |
| ----------------- | --------------------------------- |
| `sampler2D`       | Texture 2D classique              |
| `samplerCube`     | Cubemap (skybox, environment)     |
| `sampler2DShadow` | Texture 2D avec comparaison depth |
| `sampler2DArray`  | Tableau de textures 2D            |
| `sampler3D`       | Texture 3D (volumétrique)         |

### Lecture d'une texture

```glsl
uniform sampler2D albedoMap;
uniform sampler2D normalMap;

vec4 color = texture(albedoMap, TexCoord);           // échantillonnage standard
vec4 lod   = textureLod(albedoMap, TexCoord, 2.0);   // mipmap level explicite
ivec2 size = textureSize(albedoMap, 0);               // dimensions en pixels
```

### Unités de textures

Chaque sampler est lié à une **texture unit** (0, 1, 2...) :

```cpp
// C++
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, albedoTex);
shader.setInt("albedoMap", 0);    // sampler → unit 0

glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, normalTex);
shader.setInt("normalMap", 1);    // sampler → unit 1
```

---

## 11. Uniform Buffer Objects (UBO)

Partagent des données entre **plusieurs shaders** sans re-uploader pour chacun.

### GLSL

```glsl
layout (std140, binding = 0) uniform CameraData {
    mat4 view;
    mat4 projection;
    vec3 viewPos;
};
```

### Layout `std140` — règles d'alignement

| Type                     | Alignement           | Taille                 |
| ------------------------ | -------------------- | ---------------------- |
| `float` / `int` / `bool` | 4 bytes              | 4 bytes                |
| `vec2`                   | 8 bytes              | 8 bytes                |
| `vec3`                   | **16 bytes**         | 12 bytes (+ 4 padding) |
| `vec4`                   | 16 bytes             | 16 bytes               |
| `mat4`                   | 16 bytes par colonne | 64 bytes               |

⚠️ `vec3` est aligné comme `vec4` ! C'est le piège classique — beaucoup préfèrent utiliser `vec4` dans les UBO pour éviter les erreurs de padding.

### C++

```cpp
GLuint ubo;
glGenBuffers(1, &ubo);
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), nullptr, GL_DYNAMIC_DRAW);
glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);  // binding = 0

// Mise à jour
glBufferSubData(GL_UNIFORM_BUFFER, offsetof(CameraData, view), sizeof(glm::mat4), &viewMatrix);
```

---

## 12. Interface blocks

Pour grouper les `in` / `out` entre vertex et fragment shader :

```glsl
// Vertex shader
out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} vs_out;

// Fragment shader
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;
```

Le **nom du bloc** (`VS_OUT`) doit correspondre entre les stages. Le **nom de l'instance** (`vs_out` / `fs_in`) peut différer.

---

## 13. Variables built-in

### Vertex shader

| Variable        | Type    | Description                                         |
| --------------- | ------- | --------------------------------------------------- |
| `gl_VertexID`   | `int`   | Index du sommet courant                             |
| `gl_InstanceID` | `int`   | Index de l'instance (instanced rendering)           |
| `gl_Position`   | `vec4`  | **Output obligatoire** — position en clip space     |
| `gl_PointSize`  | `float` | Taille du point (si `GL_PROGRAM_POINT_SIZE` activé) |

### Fragment shader

| Variable         | Type    | Description                                                 |
| ---------------- | ------- | ----------------------------------------------------------- |
| `gl_FragCoord`   | `vec4`  | Position du fragment en window space (x, y, z=depth, w=1/w) |
| `gl_FrontFacing` | `bool`  | `true` si la face est orientée vers la caméra               |
| `gl_FragDepth`   | `float` | Override de la depth (désactive l'early-z !)                |
| `gl_PointCoord`  | `vec2`  | Coordonnée dans un point sprite [0, 1]                      |
| `gl_SampleID`    | `int`   | Index du sample (MSAA, 4.0+)                                |

---

## 14. Swizzling

Accès et réarrangement des composantes d'un vecteur :

```glsl
vec4 v = vec4(1.0, 2.0, 3.0, 4.0);

vec3 rgb = v.rgb;        // vec3(1.0, 2.0, 3.0)   — alias couleur
vec2 xy  = v.xy;         // vec2(1.0, 2.0)         — alias position
vec3 yzx = v.yzx;        // vec3(2.0, 3.0, 1.0)   — réarrangement
vec4 rrrr = v.rrrr;      // vec4(1.0, 1.0, 1.0, 1.0) — répétition

v.xz = vec2(10.0, 30.0); // écriture partielle
```

**Jeux de composantes** (interchangeables au sein d'un même jeu, mais ne pas mélanger) :

| Jeu      | Composantes | Usage typique          |
| -------- | ----------- | ---------------------- |
| Position | `x y z w`   | Coordonnées            |
| Couleur  | `r g b a`   | Couleurs / textures    |
| Texture  | `s t p q`   | Coordonnées de texture |

```glsl
// ❌ Interdit : mélanger les jeux
vec3 bad = v.xgb;   // ERREUR DE COMPILATION
```

---

## 15. Précision et conversions

### Conversions implicites

```glsl
float f = 5;       // int → float (implicite)
vec3 v = vec3(1);  // float(1) partout → vec3(1.0, 1.0, 1.0)
```

### Conversions explicites

```glsl
int i = int(3.7);       // 3 (troncature)
float f = float(i);     // 3.0
vec3 v = vec3(ivec3(1, 2, 3));
```

### Pas de cast implicite `int → float` dans les opérations

```glsl
float a = 1.0 / 3;     // ⚠️ 3 est int → division entière puis cast
float b = 1.0 / 3.0;   // ✅ = 0.333...
```

---

## 16. Bonnes pratiques

### Performance

- **Éviter `inverse()` dans le shader** — pré-calculer la normal matrix côté CPU
- **Éviter `pow()` avec des négatifs** — `pow(-x, y)` est **undefined** en GLSL
- **`normalize()` en fragment** — les normales interpolées ne sont plus unitaires
- **Branches uniformes > branches dynamiques** — `if(uniform_flag)` est gratuit, `if(varying_value)` peut casser la parallélisation
- **Minimiser les texture fetches** — chaque `texture()` a un coût de latence mémoire

### Qualité

- Toujours travailler en **espace linéaire** (gamma-correct) : convertir sRGB en linéaire à l'entrée, appliquer gamma en sortie
- Utiliser `max(dot(...), 0.0)` pour les produits scalaires de lighting — évite les contributions négatives
- Préférer `smoothstep` à `step` pour des transitions douces

### Organisation

```
resources/shaders/
├── vs_default.glsl    // vertex shader standard
├── fs_pbr.glsl        // fragment shader PBR
├── vs_light.glsl      // vertex pour debug lights
├── fs_light.glsl      // fragment pour debug lights
├── vs_depth.glsl      // vertex pour depth pass
└── fs_depth.glsl      // fragment pour depth/shadow
```

---

## 17. Différences 3.3 → 4.1

| Feature                       | 3.3 (330)            | 4.0 (400) | 4.1 (410) |
| ----------------------------- | -------------------- | --------- | --------- |
| UBO                           | ✅                   | ✅        | ✅        |
| Instanced rendering           | ✅ (`gl_InstanceID`) | ✅        | ✅        |
| MRT (Multiple Render Targets) | ✅                   | ✅        | ✅        |
| Tessellation                  | ❌                   | ✅        | ✅        |
| `double` precision            | ❌                   | ✅        | ✅        |
| Subroutines                   | ❌                   | ✅        | ✅        |
| `glProgramUniform*`           | ❌                   | ❌        | ✅        |
| Séparation shader stages      | ❌                   | ❌        | ✅        |
| `layout(location) uniform`    | ❌                   | ❌        | ✅        |

Pour ProutEngine, rester en **`#version 410 core`** permet d'accéder au `layout(location)` sur les uniforms tout en restant compatible macOS (max 4.1).

---

## Annexe — Template PBR minimal

Référence pour les fonctions PBR utilisées dans le projet (voir `fs_pbr.glsl`) :

```glsl
// Distribution GGX/Trowbridge-Reitz
// Mesure la concentration des microfacettes alignées avec le halfway vector
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a  = roughness * roughness;     // remapping carré (Disney/UE4)
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

// Géométrie Schlick-GGX (une direction)
// Modélise l'auto-occlusion des microfacettes
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;    // remapping pour lighting direct

    return NdotV / (NdotV * (1.0 - k) + k);
}

// Géométrie Smith (deux directions : vue + lumière)
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) *
           GeometrySchlickGGX(NdotL, roughness);
}

// Fresnel Schlick
// Plus on regarde en rasant, plus la réflexion augmente
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
```

**Paramètres PBR d'une surface :**

| Paramètre   | Type    | Range  | Description                                               |
| ----------- | ------- | ------ | --------------------------------------------------------- |
| `albedo`    | `vec3`  | [0, 1] | Couleur de base (sRGB → linéaire)                         |
| `metallic`  | `float` | [0, 1] | 0 = diélectrique, 1 = métal                               |
| `roughness` | `float` | [0, 1] | 0 = miroir parfait, 1 = diffus total                      |
| `ao`        | `float` | [0, 1] | Ambient occlusion                                         |
| `F0`        | `vec3`  | —      | Réflectance à incidence normale (0.04 pour diélectriques) |
