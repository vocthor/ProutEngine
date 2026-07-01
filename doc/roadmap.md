# ProutEngine — Roadmap d'implémentation

Ce document décrit les phases d'implémentation pour migrer l'architecture actuelle vers l'architecture cible décrite dans `architecture.md`. Chaque phase est indépendamment vérifiable.

**Légende** : ⬜ À faire — 🚧 En cours — ✅ Terminé

---

## Phase 0 — Corrections critiques (sur le code actuel)

**Objectif** : Corriger les bugs et incohérences avant de refactorer.

| Statut | #   | Tâche                                                                                                                                                                                          | Fichiers impactés                                                                        |
| ------ | --- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------- |
| ✅     | 0.1 | Uniformiser RAII : appliquer `AutoRelease` à `VAO`, `VBO`, `EBO`. Supprimer toutes les méthodes `remove()`.                                                                                    | `VAO.hpp/cpp`, `VBO.hpp/cpp`, `EBO.hpp/cpp`, `mesh.hpp/cpp`, `texture.hpp/cpp`           |
| ✅     | 0.2 | Stocker `VBO` et `EBO` en tant que membres de `Mesh` (au lieu de locaux dans le constructeur).                                                                                                 | `mesh.hpp`, `mesh.cpp`                                                                   |
| ✅     | 0.3 | Corriger le ownership des `Texture` : introduire un `TextureManager` centralisé avec cache par chemin. `Mesh` et `Model` utilisent des `TextureHandle` (index opaque) au lieu de `shared_ptr`. | `textureManager.hpp/cpp`, `texture.hpp/cpp`, `mesh.hpp/cpp`, `model.hpp/cpp`, `main.cpp` |
| ✅     | 0.4 | Supprimer `Texture::remove()` et `Texture::texUnit()`. Activer `AutoRelease` sur `Texture`. Le cleanup GPU est géré par RAII.                                                                  | `texture.hpp/cpp`                                                                        |
| ✅     | 0.5 | Utiliser `std::move` dans le constructeur de `Mesh` au lieu de copier les vecteurs.                                                                                                            | `mesh.cpp`                                                                               |
| ✅     | 0.6 | Uniformiser `#pragma once` partout.                                                                                                                                                            | `camera.hpp`, `mesh.hpp`, `VAO.hpp`, `EBO.hpp`                                           |
| ⬜     | 0.7 | Remplacer `std::cout` par `std::cerr` pour les erreurs. Remplacer les `assert` en release par des `if + throw`.                                                                                | `shaderProgram.cpp`, `model.cpp`, `texture.cpp`                                          |

**Vérification** : Le projet compile et s'exécute comme avant (`make run`).

---

## Phase 1 — Fondations (`core/`, `utility/`)

**Objectif** : Extraire la boucle principale de `main.cpp`.

| Statut | #   | Tâche                                        | Détail                                                                                                                                                      |
| ------ | --- | -------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ✅     | 1.1 | Créer `Log` (`utility/log.hpp`)              | Fonctions `Log::info`, `Log::error`, etc. avec `std::format`. Remplacer tous les `std::cout`/`std::cerr` du projet.                                         |
| ✅     | 1.2 | Créer `Window` (`core/window.hpp`)           | Wrapper RAII de `GLFWwindow*`. Constructeur = `glfwCreateWindow`. Destructeur = `glfwDestroyWindow`.                                                        |
| ✅     | 1.3 | Créer `Timer` (`core/timer.hpp`)             | Struct simple : `update()`, `deltaTime()`. Remplace les variables globales `deltaTime`/`lastFrame`.                                                         |
| ✅     | 1.4 | Créer `Application` (`core/application.hpp`) | Contient `Window`, `Timer`. Méthode `run()` = boucle principale extraite de `main()`.                                                                       |
| ✅     | 1.5 | Réduire `main.cpp`                           | `main()` ne fait plus que `Application app; app.run();`                                                                                                     |
| 🚧     | 1.6 | Formes basiques                              | Remettre en place un système pour avoir des cubes facilement (cf main avant commit 7f8327454153e126a58742ccaf19f1d39dbbe904) -> factory ? simple fonction ? |

**Vérification** : Le rendu est identique. `main.cpp` < 10 lignes.

---

## Phase 2 — Input (`input/`)

**Objectif** : Centraliser la gestion des inputs.

| Statut | #   | Tâche                                                   | Détail                                                                                                                                   |
| ------ | --- | ------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------- |
| ✅     | 2.1 | Créer `InputManager` (`input/inputManager.hpp`)         | Encapsule `glfwGetKey` / `glfwGetMouseButton` / `glfwGetCursorPos`. Stocke l'état courant et précédent pour détecter les front montants. |
| ✅     | 2.2 | Créer `CameraController` (`scene/cameraController.hpp`) | Déplace la logique de `Camera::inputs()` ici. Consomme `InputManager` au lieu de `GLFWwindow*`.                                          |
| ✅     | 2.3 | Supprimer `processInput()` de `main.cpp`                | L'échap est géré par `InputManager` / `Application`.                                                                                     |
| ✅     | 2.4 | Nettoyer `Camera`                                       | Retirer toute dépendance GLFW de `Camera`. Elle ne fait que stocker position/direction et retourner des matrices.                        |

**Vérification** : WASD + souris fonctionnent comme avant. `Camera` n'inclut plus `<GLFW/glfw3.h>`.

---

## Phase 3 — Scene & Lumières (`scene/`)

**Objectif** : Structurer la scène et les lumières en objets.

| Statut | #   | Tâche                                             | Détail                                                                                                   |
| ------ | --- | ------------------------------------------------- | -------------------------------------------------------------------------------------------------------- |
| ✅     | 3.1 | Créer `Transform` (`scene/transform.hpp`)         | Struct avec position, rotation, scale. Méthode `getModelMatrix()` avec dirty flag et cache.              |
| ✅     | 3.2 | Créer les structs de lumières (`scene/light.hpp`) | `DirectionalLight`, `PointLight`, `SpotLight`. Namespace `LightUtils` pour uploader vers un shader.      |
| ✅     | 3.3 | Créer `Entity` (`scene/entity.hpp`)               | Contient un `Transform`, un `Model*`, un `Material*` (forward decl, Phase 4.1), un nom.                  |
| ✅     | 3.4 | Créer `Scene` (`scene/scene.hpp`)                 | Conteneur d'`Entity` + `Camera` + lumières typées. Méthode `uploadLights(ShaderProgram&)`.               |
| ✅     | 3.5 | Migrer le code lumière de `main.cpp`              | Lumières déclarées dans `Scene` via `initSceneLights()`. Upload via `scene.uploadLights(shaderProgram)`. |

**Vérification** : La scène avec le backpack et les 2 point lights s'affiche identiquement.

---

## Phase 4 — Material & Renderer (`rendering/`)

**Objectif** : Découpler le rendu de la géométrie.

| Statut | #   | Tâche                                                     | Détail                                                                                                                                                                                                                                                                                                                                                                                                                                                               |
| ------ | --- | --------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ✅     | 4.0 | Créer `GPUBuffer<Target>` (`rendering/gpu/gpuBuffer.hpp`) | Template `GLenum Target`. `VBO` et `EBO` deviennent des alias (`using VBO = GPUBuffer<GL_ARRAY_BUFFER>`). Prérequis pour 4.7 (UBO).                                                                                                                                                                                                                                                                                                                                  |
| ✅     | 4.1 | Créer `Material` (`rendering/material.hpp`)               | Associe un `ShaderProgram` à des `Texture`s nommées ("albedo", "normal", "metallic"...) et des propriétés. Méthode `bind()`. Prérequis pour 4.2.                                                                                                                                                                                                                                                                                                                     |
| ✅     | 4.2 | Refondre les lumières — modèle `color + intensity`        | Unifier `DirectionalLight/PointLight/SpotLight` en un seul `struct Light { color, intensity, type, ... }`. Tableau `uniform Light lights[MAX_LIGHTS]` + `uniform int numLights` dans le shader. Supprimer `ambient/diffuse/specular` par lumière, remplacé par un `uniform vec3 ambientColor` dans `Scene`. Passer tous les shaders à `#version 410 core`. Ajouter gamma correction (Reinhard + `pow(1/2.2)`) en sortie du fragment shader. **Prérequis pour 6.11.** |
| ✅     | 4.3 | Modifier `Mesh`                                           | Supprimer le vecteur de `Texture` et le paramètre `Camera` de `draw()`. `Mesh::draw()` ne fait que `glDrawElements`.                                                                                                                                                                                                                                                                                                                                                 |
| ✅     | 4.4 | Ajouter le cache d'uniforms à `ShaderProgram`             | `std::unordered_map<std::string, GLint>` dans `ShaderProgram`.                                                                                                                                                                                                                                                                                                                                                                                                       |
| ✅     | 4.5 | Créer `Renderer` (`rendering/renderer.hpp`)               | Accepte des `RenderCommand` (mesh + material + transform), trie par shader/material, dessine. Le renderer décompose `Entity → Model → [Mesh × Material]` pour construire les `RenderCommand`.                                                                                                                                                                                                                                                                        |
| ✅     | 4.6 | Migrer la boucle de rendu                                 | `Application::run()` appelle `renderer.beginFrame()`, soumet les entités de la scène (le renderer résout chaque `Model*` en draw calls), `renderer.endFrame()`.                                                                                                                                                                                                                                                                                                      |
| ⬜     | 4.7 | UBO caméra (`render/cameraUBO.hpp`)                       | Factoriser `view`, `projection`, `viewPos` dans un Uniform Buffer Object (binding 0) partagé par tous les shaders. Évite de re-setter 3 uniforms par draw call. Requiert `#version 410 core` (4.2).                                                                                                                                                                                                                                                                  |

**Vérification** : Rendu identique. `Mesh` n'a plus de dépendance sur `Camera` ni `Texture`.

---

## Phase 5 — ResourceManager (`resource/`)

**Objectif** : Centraliser le chargement et le cache des ressources.

| Statut | #   | Tâche                                  | Détail                                                                                                                                                        |
| ------ | --- | -------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ⬜     | 5.1 | Créer `ResourceManager`                | `loadTexture(path) → TextureHandle` et `loadModel(path) → ModelHandle` avec cache par chemin. `ResourceManager` possède tous les assets (`Model`, `Texture`). |
| ⬜     | 5.2 | Migrer `Model`                         | `Model` prend un `ResourceManager&` au lieu de gérer `loadedTextures_` lui-même. `Model` reste la structure interne gérant ses `Mesh`es.                      |
| ⬜     | 5.3 | Supprimer `loadedTextures_` de `Model` | Le cache de textures est dans `ResourceManager`.                                                                                                              |
| ⬜     | 5.4 | Migrer `Entity` vers `ModelHandle`     | `Entity::model` passe de `Model*` à `ModelHandle` (index opaque). Deux entités du même asset partagent la géométrie GPU sans duplication.                     |

**Vérification** : Charger 2 fois le même modèle ne duplique pas les textures GPU. Deux entités référençant le même asset partagent le même `Model` en mémoire.

---

## Phase 6 — Extensions (fonctionnalités nouvelles)

À implémenter dans l'ordre souhaité, après les phases 0-5.

| Statut | #    | Fonctionnalité                 | Complexité | Détail                                                                                                                                                                                                                                                                                                     |
| ------ | ---- | ------------------------------ | ---------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ⬜     | 6.1  | **Framebuffer**                | Moyenne    | Classe `Framebuffer` RAII. Permet le rendu offscreen. Prérequis pour 6.2, 6.3, 6.13.                                                                                                                                                                                                                       |
| ⬜     | 6.2  | **Post-processing**            | Moyenne    | Rendu dans un FBO, puis quad plein écran avec shader de post-process (bloom, vignette...). Prérequis : 6.1.                                                                                                                                                                                                |
| ⬜     | 6.3  | **Shadow mapping**             | Élevée     | Depth pass depuis le POV de la lumière dans un FBO, puis sampling PCF dans le fragment shader. Prérequis : 6.1.                                                                                                                                                                                            |
| ⬜     | 6.4  | **Skybox**                     | Faible     | Cubemap + shader dédié, rendu en dernier avec depth test ≤. Prérequis pour 6.12 (IBL).                                                                                                                                                                                                                     |
| ✅     | 6.5  | **Normal mapping**             | Moyenne    | Ajouter tangent/bitangent à `Vertex`, calculer via Assimp (`aiProcess_CalcTangentSpace`), sampler la normal map en tangent space. **Prérequis pour 6.11.**                                                                                                                                                 |
| ⬜     | 6.6  | **ImGui**                      | Faible     | Intégrer Dear ImGui pour un panneau de debug (FPS, positions lumières, propriétés matériaux, scène).                                                                                                                                                                                                       |
| ⬜     | 6.7  | **Sérialisation de scène**     | Moyenne    | Charger/sauvegarder une scène en JSON (nlohmann/json) : entités, transforms, matériaux, lumières.                                                                                                                                                                                                          |
| ⬜     | 6.8  | **ECS**                        | Élevée     | Remplacer `Entity` par un vrai ECS (ex: entt). Components = `Transform`, `MeshComponent`, `MaterialComponent`, `LightComponent`.                                                                                                                                                                           |
| ⬜     | 6.9  | **Instanced rendering**        | Moyenne    | `glDrawElementsInstanced` pour dessiner N fois le même mesh avec des transforms différents.                                                                                                                                                                                                                |
| ⬜     | 6.10 | **Frustum culling**            | Moyenne    | Calculer le frustum depuis la VP matrix, tester les bounding boxes des entités avant de les soumettre au renderer.                                                                                                                                                                                         |
| ✅     | 6.11 | **PBR — Cook-Torrance BRDF**   | Élevée     | Créer `fs_pbr.glsl` avec BRDF microfacette : distribution GGX, géométrie Smith, Fresnel-Schlick. refacto `struct Material` (albedoMap, normalMap, metallicMap, roughnessMap, aoMap). Le shader itère sur le tableau `lights[]` de 4.2. Prérequis : **4.2** (lumières unifiées) + **6.5** (normal mapping). |
| ⬜     | 6.12 | **IBL (Image-Based Lighting)** | Élevée     | Précalculer une irradiance map (diffuse) et une prefiltered env map (spéculaire) depuis une HDRI au chargement. Ajouter une BRDF LUT. Remplace l'`ambientColor` global. Prérequis : **6.4** (cubemap) + **6.11** (PBR BRDF).                                                                               |
| ⬜     | 6.13 | **HDR pipeline + tonemapping** | Moyenne    | Rendu dans un FBO HDR (`GL_RGB16F`). Tonemapping ACES ou Filmic en post-process (remplace le Reinhard in-shader de 4.2). Contrôle exposition. Prérequis : **6.1**, **6.11**.                                                                                                                               |

---

## Résumé visuel des phases

```
Phase 0 ───────► Phase 1 ───────► Phase 2
(bug fixes)     (core/app)       (input)
                    │
                    ▼
Phase 3 ───────► Phase 4 ───────► Phase 5
(scene)         (renderer)       (resources)
                    │
                    ▼
              Phase 6 (extensions)

  Dépendances PBR dans Phase 6 :
  6.5 (normal map) ──┐
  4.2 (lumières)   ──┼──► 6.11 (PBR BRDF) ──► 6.12 (IBL)
  6.4 (cubemap)    ──┘              │
  6.1 (FBO)   ─────────────────────► 6.13 (HDR)
```

Les phases 1 et 2 peuvent être faites en parallèle. Les phases 3, 4, 5 sont séquentielles.
