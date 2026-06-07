# ProutEngine — Architecture cible

## Vue d'ensemble

```
┌──────────────────────────────────────────────────────────────────┐
│                          Application                             │
│  (initialisation, boucle principale, gestion de la fenêtre)      │
├──────────────────────────────────────────────────────────────────┤
│                            Engine                                │
│  ┌──────────┐  ┌──────────┐  ┌───────────┐  ┌───────────────┐   │
│  │ Renderer │  │  Scene   │  │   Input   │  │   Resource    │   │
│  │          │  │  Graph   │  │  Manager  │  │   Manager     │   │
│  └────┬─────┘  └────┬─────┘  └───────────┘  └───────┬───────┘   │
│       │              │                               │           │
│  ┌────┴─────┐  ┌─────┴──────────────┐        ┌──────┴───────┐   │
│  │ Render   │  │  Entity/Component  │        │  Cache<T>    │   │
│  │ Pipeline │  │  System (ECS)      │        │  (textures,  │   │
│  └────┬─────┘  └────────────────────┘        │   shaders,   │   │
│       │                                      │   modèles)   │   │
│  ┌────┴──────────────────────────────┐       └──────────────┘   │
│  │        OpenGL Abstraction         │                           │
│  │  Shader, ShaderProgram, Texture,  │                           │
│  │  VAO, VBO, EBO, Framebuffer, UBO  │                           │
│  └───────────────────────────────────┘                           │
└──────────────────────────────────────────────────────────────────┘
```

---

## 1. Module Core

### 1.1 `Application`

Point d'entrée du moteur. Responsable du cycle de vie global.

```
class Application
├── run()                    # Boucle principale
├── init()                   # Init GLFW, GLAD, contexte OpenGL
├── shutdown()               # Nettoyage
├── onResize(width, height)  # Callback resize
│
├── Window       window_
├── Engine       engine_
├── InputManager inputManager_
└── Timer        timer_
```

**Responsabilités** :

- Créer la fenêtre GLFW (via `Window`)
- Initialiser GLAD
- Orchestrer la boucle `while (!shouldClose)` : input → update → render
- Propager les événements (resize, close) aux sous-systèmes

### 1.2 `Window`

Wrapper RAII autour de `GLFWwindow*`.

```
class Window
├── Window(width, height, title)
├── ~Window()                    # glfwDestroyWindow
├── shouldClose() → bool
├── swapBuffers()
├── pollEvents()
├── getFramebufferSize() → (int, int)
├── setResizeCallback(fn)
│
├── GLFWwindow* handle_          # (privé)
├── int width_, height_
└── std::string title_
```

**Détail** : Le constructeur appelle `glfwInit()` via un singleton `GLFWContext` (ou compteur de références statique) pour garantir un seul init/terminate.

### 1.3 `Timer`

Gestion du temps frame-à-frame.

```
struct Timer
├── update()
├── deltaTime() → float
├── elapsed()   → float
├── fps()       → float
│
├── float deltaTime_
├── float lastFrame_
└── float elapsed_
```

---

## 2. Module Input

### 2.1 `InputManager`

Centralise la gestion des entrées clavier et souris.

```
class InputManager
├── processEvents(Window&)
├── isKeyPressed(Key) → bool
├── isKeyJustPressed(Key) → bool
├── isMouseButtonPressed(MouseButton) → bool
├── getMousePosition() → glm::vec2
├── getMouseDelta() → glm::vec2
├── setCursorMode(CursorMode)
│
├── std::array<bool, KEY_COUNT> currentKeys_
├── std::array<bool, KEY_COUNT> previousKeys_
├── glm::vec2 mousePos_
├── glm::vec2 mouseDelta_
└── bool firstMouse_
```

**Détail** : Remplace les appels `glfwGetKey` dispersés dans `Camera` et `main()`. Permet d'ajouter facilement du key-binding, des actions mappées, etc.

### 2.2 `enum class Key` / `enum class MouseButton`

Abstraction au-dessus des constantes GLFW pour découpler le moteur de la bibliothèque de fenêtrage.

---

## 3. Module Rendering

### 3.1 OpenGL Abstraction Layer (bas niveau)

Toutes ces classes utilisent `AutoRelease<GLuint>` systématiquement.

#### `GPUBuffer`

Classe template de base pour VBO et EBO.

```
template<GLenum Target>
class GPUBuffer
├── GPUBuffer(std::span<const std::byte> data, GLenum usage = GL_STATIC_DRAW)
├── bind()
├── unbind()
├── update(std::span<const std::byte> data, GLintptr offset = 0)
│
└── AutoRelease<GLuint> handle_
```

- `using VBO = GPUBuffer<GL_ARRAY_BUFFER>;`
- `using EBO = GPUBuffer<GL_ELEMENT_ARRAY_BUFFER>;`
- `using UBO = GPUBuffer<GL_UNIFORM_BUFFER>;`

#### `VAO`

```
class VAO
├── VAO()
├── bind()
├── unbind()
├── addAttribute(GLuint location, GLint size, GLenum type, GLsizei stride, GLintptr offset)
│
└── AutoRelease<GLuint> handle_
```

#### `Shader` (inchangé, bon tel quel)

```
class Shader
├── Shader(std::string_view path, ShaderType type)
├── type() → ShaderType
├── nativeHandle() → GLuint
│
├── AutoRelease<GLuint> handle_
└── ShaderType type_
```

#### `ShaderProgram` (amélioré)

```
class ShaderProgram
├── ShaderProgram(const Shader& vertex, const Shader& fragment)
├── use()
├── setUniform(std::string_view name, T value)  # template unique
├── getUniformLocation(std::string_view name) → GLint  # avec cache
│
├── AutoRelease<GLuint> handle_
└── std::unordered_map<std::string, GLint> uniformCache_
```

**Amélioration clé** : Le cache d'uniforms évite les appels répétés à `glGetUniformLocation`. La méthode `setUniform` est un template unique avec `if constexpr` pour dispatcher selon le type.

#### `Texture`

```
class Texture
├── Texture(std::string_view path)
├── bind(GLuint unit)
├── unbind()
├── handle() → GLuint
├── path() → std::string_view
│
├── AutoRelease<GLuint> handle_
├── int width_, height_, channels_
├── GLenum format_
└── std::string path_
```

**Changement** : Plus de `type` (diffuse/specular) dans `Texture`. C'est le `Material` qui décide quel rôle joue la texture.

#### `Framebuffer`

```
class Framebuffer
├── Framebuffer(int width, int height, std::vector<AttachmentDesc> attachments)
├── bind()
├── unbind()
├── resize(int width, int height)
├── getColorTexture(int index) → GLuint
├── getDepthTexture() → GLuint
│
├── AutoRelease<GLuint> handle_
├── std::vector<AutoRelease<GLuint>> colorAttachments_
└── AutoRelease<GLuint> depthAttachment_

struct AttachmentDesc
├── GLenum internalFormat   # GL_RGBA8, GL_DEPTH_COMPONENT24, etc.
├── GLenum type             # GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT
```

Nécessaire pour le post-processing, les shadow maps, le deferred rendering.

---

### 3.2 Primitives de rendu (haut niveau)

#### `Vertex`

```
struct Vertex
├── glm::vec3 position
├── glm::vec3 normal
├── glm::vec2 texCoords
├── glm::vec3 tangent      # (futur : normal mapping)
└── glm::vec3 bitangent    # (futur : normal mapping)
```

**Changement** : Suppression de `color` (rarement utilisé en pratique, les couleurs viennent des matériaux/textures). Ajout futur de tangent/bitangent pour le normal mapping.

#### `Mesh`

```
class Mesh
├── Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices)
├── draw()           # glDrawElements uniquement — pas de shader, pas de caméra
├── vertexCount() → size_t
├── indexCount() → size_t
│
├── VAO vao_
├── VBO vbo_         # ← STOCKÉ en membre (corrige le bug actuel)
├── EBO ebo_         # ← STOCKÉ en membre
└── size_t indexCount_
```

**Changement majeur** : `Mesh` ne connaît **ni** les shaders, **ni** les textures, **ni** la caméra. Il ne fait que dessiner de la géométrie. Le couplage shader/texture est géré par `Material` et `Renderer`.

#### `Material`

```
class Material
├── Material(std::shared_ptr<ShaderProgram> shader)
├── setTexture(std::string_view slot, std::shared_ptr<Texture> texture)
├── setProperty(std::string_view name, T value)
├── bind()           # active le shader + bind toutes les textures + set uniforms
├── unbind()
│
├── std::shared_ptr<ShaderProgram> shader_
├── std::unordered_map<std::string, std::shared_ptr<Texture>> textures_
│   # ex: {"diffuse" → tex1, "specular" → tex2, "normal" → tex3}
├── float shininess_
└── std::unordered_map<std::string, UniformValue> properties_
     # UniformValue = std::variant<float, int, glm::vec3, glm::vec4, glm::mat4>
```

**Rôle** : Un `Material` associe un `ShaderProgram` à des `Textures` et des propriétés (shininess, couleur, etc.). Plusieurs `Mesh` peuvent partager le même `Material`.

---

### 3.3 Renderer

#### `Renderer`

Orchestre le pipeline de rendu complet.

```
class Renderer
├── init(int width, int height)
├── beginFrame()
├── endFrame()
├── submit(const RenderCommand& cmd)    # ajoute à la file de rendu
├── flush()                             # exécute tous les draw calls
├── resize(int width, int height)
│
├── std::vector<RenderCommand> renderQueue_
├── Framebuffer framebuffer_            # (optionnel, post-processing)
└── glm::vec4 clearColor_

struct RenderCommand
├── Mesh* mesh
├── Material* material
├── glm::mat4 modelMatrix
├── glm::mat3 normalMatrix
```

**Pipeline de rendu** :

```
beginFrame()
  → glClear

Pour chaque RenderCommand (trié par shader, puis par matériau) :
  → material.bind()           # active shader + textures
  → shader.setUniforms(...)   # view, projection, model, lights
  → mesh.draw()               # glDrawElements

endFrame()
  → post-processing (optionnel)
  → glfwSwapBuffers
```

**Avantage du tri** : Minimise les changements d'état GPU (changements de shader, de texture) — gain de performance significatif.

---

## 4. Module Scene

### 4.1 `Camera`

```
class Camera
├── Camera(float fov, float aspect, float near, float far)
├── setPosition(glm::vec3)
├── setDirection(glm::vec3)
├── getViewMatrix() → glm::mat4
├── getProjectionMatrix() → glm::mat4
├── getViewProjectionMatrix() → glm::mat4
├── updateAspect(float aspect)
│
├── glm::vec3 position_
├── glm::vec3 direction_
├── glm::vec3 up_
├── float fov_, nearPlane_, farPlane_, aspect_
├── glm::mat4 viewMatrix_           # cache, recalculé si dirty
├── glm::mat4 projectionMatrix_     # cache, recalculé si dirty
└── bool viewDirty_, projDirty_
```

**Changements** :

- La caméra **ne connaît plus** `ShaderProgram`. Elle retourne des matrices, c'est tout.
- Les matrices sont cachées (dirty flag) — la projection n'est recalculée que quand le FOV/aspect change.
- Les inputs sont gérés par un `CameraController` séparé (voir ci-dessous).

### 4.2 `CameraController`

```
class CameraController
├── CameraController(Camera& camera, InputManager& input)
├── update(float deltaTime)
├── setSpeed(float)
├── setSensitivity(float)
│
├── Camera& camera_
├── InputManager& input_
├── float speed_
└── float sensitivity_
```

Séparer les contrôles de la caméra elle-même permet d'avoir des modes différents : FPS, orbit, cinématique, etc.

### 4.3 Lumières

```
struct DirectionalLight
├── glm::vec3 direction
├── glm::vec3 ambient
├── glm::vec3 diffuse
└── glm::vec3 specular

struct PointLight
├── glm::vec3 position
├── glm::vec3 ambient
├── glm::vec3 diffuse
├── glm::vec3 specular
├── float constant
├── float linear
└── float quadratic

struct SpotLight
├── glm::vec3 position
├── glm::vec3 direction
├── glm::vec3 ambient
├── glm::vec3 diffuse
├── glm::vec3 specular
├── float constant, linear, quadratic
├── float cutOff
└── float outerCutOff
```

Ces structs sont de simples données. Leur envoi au shader est géré par un utilitaire :

```
namespace LightUtils
├── uploadToShader(ShaderProgram&, const DirectionalLight&)
├── uploadToShader(ShaderProgram&, const PointLight&, int index)
├── uploadToShader(ShaderProgram&, const SpotLight&)
```

### 4.4 `Scene`

```
class Scene
├── addEntity(std::unique_ptr<Entity>) → Entity&
├── removeEntity(Entity&)
├── getEntities() → std::span<std::unique_ptr<Entity>>
├── setCamera(Camera)
├── getCamera() → Camera&
├── addLight(LightVariant)       # std::variant<DirectionalLight, PointLight, SpotLight>
├── getLights() → const LightData&
│
├── std::vector<std::unique_ptr<Entity>> entities_
├── Camera camera_
└── LightData lights_

struct LightData
├── std::optional<DirectionalLight> directional
├── std::vector<PointLight> pointLights      # MAX = 16 (ou dynamique via SSBO)
└── std::vector<SpotLight> spotLights
```

### 4.5 `Entity` (version simple) → futur ECS

```
class Entity
├── Entity(std::string name)
├── setTransform(Transform)
├── getTransform() → Transform&
├── setMesh(std::shared_ptr<Mesh>)
├── setMaterial(std::shared_ptr<Material>)
├── getMesh() → Mesh*
├── getMaterial() → Material*
├── isVisible() → bool
│
├── std::string name_
├── Transform transform_
├── std::shared_ptr<Mesh> mesh_
├── std::shared_ptr<Material> material_
└── bool visible_
```

### 4.6 `Transform`

```
struct Transform
├── glm::vec3 position   = {0, 0, 0}
├── glm::vec3 rotation   = {0, 0, 0}   # Euler en degrés
├── glm::vec3 scale      = {1, 1, 1}
│
├── getModelMatrix() → glm::mat4
├── getNormalMatrix() → glm::mat3
├── setDirty()
│
├── glm::mat4 cachedModel_      # cache
└── bool dirty_
```

---

## 5. Module Resource

### 5.1 `ResourceManager`

Gère le chargement, le cache et la durée de vie de toutes les ressources GPU.

```
class ResourceManager
├── loadTexture(std::string_view path) → std::shared_ptr<Texture>
├── loadShader(std::string_view vtx, std::string_view frag) → std::shared_ptr<ShaderProgram>
├── loadModel(std::string_view path) → std::shared_ptr<Model>
├── clear()                   # libère tout
│
├── std::unordered_map<std::string, std::shared_ptr<Texture>> textures_
├── std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shaders_
└── std::unordered_map<std::string, std::shared_ptr<Model>> models_
```

**Rôle** : Évite de charger 2 fois la même texture (remplace le `loadedTextures_` de `Model`). Ownership clair via `shared_ptr`.

### 5.2 `Model` (redéfini)

```
class Model
├── Model(std::string_view path, ResourceManager& resources)
├── getMeshes() → std::span<MeshEntry>
│
└── std::vector<MeshEntry> entries_

struct MeshEntry
├── std::shared_ptr<Mesh> mesh
└── std::shared_ptr<Material> material
```

Le `Model` chargé via Assimp produit des paires `(Mesh, Material)`. Le `ResourceManager` gère le cache de textures sous-jacent.

---

## 6. Module Utility

### 6.1 `AutoRelease<T>` (inchangé — bon tel quel)

### 6.2 `Log`

```
namespace Log
├── info(std::format_string, Args...)
├── warn(std::format_string, Args...)
├── error(std::format_string, Args...)
├── debug(std::format_string, Args...)
```

Remplace les `std::cout` dispersés. Peut écrire vers la console et/ou un fichier. Utilise `std::format` (C++23).

---

## 7. Diagramme de dépendances entre modules

```
Application
  ├── Window
  ├── Timer
  ├── InputManager
  ├── Engine
  │     ├── Renderer
  │     │     ├── ShaderProgram
  │     │     ├── Framebuffer
  │     │     ├── VAO / VBO / EBO
  │     │     └── Texture (via Material)
  │     ├── Scene
  │     │     ├── Camera
  │     │     ├── Entity → Transform, Mesh, Material
  │     │     └── Lights (Directional, Point, Spot)
  │     └── ResourceManager
  │           ├── Cache<Texture>
  │           ├── Cache<ShaderProgram>
  │           └── Cache<Model>
  └── CameraController (← Camera + InputManager)
```

**Règle de dépendance** : Les flèches vont vers le bas. Aucun module bas-niveau ne connaît les modules haut-niveau. `Mesh` ne connaît pas `Camera`. `Texture` ne connaît pas `ShaderProgram`. `Camera` ne connaît pas `ShaderProgram`.

---

## 8. Organisation des fichiers cible

```
ProutEngine/
├── CMakeLists.txt
├── doc/
│   ├── architecture.md          ← ce fichier
│   └── roadmap.md               ← phases d'implémentation
├── include/
│   ├── core/
│   │   ├── application.hpp
│   │   ├── window.hpp
│   │   └── timer.hpp
│   ├── input/
│   │   └── inputManager.hpp
│   ├── rendering/
│   │   ├── shader.hpp
│   │   ├── shaderProgram.hpp
│   │   ├── texture.hpp
│   │   ├── material.hpp
│   │   ├── mesh.hpp
│   │   ├── renderer.hpp
│   │   ├── framebuffer.hpp
│   │   └── gpu/
│   │       ├── vao.hpp
│   │       ├── gpuBuffer.hpp    ← template VBO/EBO/UBO
│   │       └── autoRelease.hpp
│   ├── scene/
│   │   ├── camera.hpp
│   │   ├── cameraController.hpp
│   │   ├── entity.hpp
│   │   ├── transform.hpp
│   │   ├── light.hpp
│   │   └── scene.hpp
│   ├── resource/
│   │   ├── resourceManager.hpp
│   │   └── model.hpp
│   └── utility/
│       └── log.hpp
├── src/
│   ├── core/
│   ├── input/
│   ├── rendering/
│   │   └── gpu/
│   ├── scene/
│   ├── resource/
│   └── main.cpp               ← réduit à ~20 lignes
├── resources/
│   ├── shaders/
│   ├── models/
│   └── textures/
└── lib/
```
