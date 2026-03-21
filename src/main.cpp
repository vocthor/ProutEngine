

#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils/autoRelease.hpp"
#include "core/timer.hpp"
#include "core/window.hpp"
#include "input/inputManager.hpp"
#include "render/mesh.hpp"
#include "scene/light.hpp"
#include "utils/log.hpp"
#include "model.hpp"
#include "cameraController.hpp"
#include "textureManager.hpp"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Vertices coordinates
// * Need 24 vertexes because normals cannot be shared between faces
Vertex cubeVertices[] = {
    //           COORDINATES            /            NORMAL           / COLOR (useless) /      TEXTURE COORD     //
    // Front
    Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)}, // Lower left front corner
    Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f)},  // Upper left front corner
    Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)},   // Upper right front corner
    Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f)},  // Lower right front corner
    // Back
    Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f)}, // Lower left back corner
    Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)},  // Upper left back corner
    Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f)},   // Upper right back corner
    Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)},  // Lower right back corner
    // Lower
    Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)},  // Lower left front corner
    Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f)},   // Lower right front corner
    Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)},  // Lower right back corner
    Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f)}, // Lower left back corner
    // Upper
    Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f)},  // Upper left front corner
    Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)},   // Upper right front corner
    Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f)},  // Upper right back corner
    Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)}, // Upper left back corner
    // Right
    Vertex{glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)},   // Upper right front corner
    Vertex{glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f)},  // Lower right front corner
    Vertex{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)}, // Lower right back corner
    Vertex{glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f)},  // Upper right back corner
    // Left
    Vertex{glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 0.0f)},  // Lower left front corner
    Vertex{glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(0.0f, 1.0f)},   // Upper left front corner
    Vertex{glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 1.0f)},  // Upper left back corner
    Vertex{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec2(1.0f, 0.0f)}, // Lower left back corner
};

// Indices for cubeVertices order
GLuint cubeIndices[] = {
    // Front
    0, 1, 2,
    0, 2, 3,
    // Back
    4, 5, 6,
    4, 6, 7,
    // Lower
    8, 9, 10,
    8, 10, 11,
    // Upper
    12, 13, 14,
    12, 14, 15,
    // Right
    16, 17, 18,
    16, 18, 19,
    // Left
    20, 21, 22,
    20, 22, 23};

// world space positions of our cubes
glm::vec3 cubePositions[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f, 3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f, 2.0f, -2.5f),
    glm::vec3(1.5f, 0.2f, -1.5f),
    glm::vec3(-1.3f, 1.0f, -1.5f),
};

Vertex lightVertices[] = {
    //     COORDINATES     //
    Vertex{glm::vec3(-0.1f, -0.1f, 0.1f)},
    Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
    Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
    Vertex{glm::vec3(0.1f, -0.1f, 0.1f)},
    Vertex{glm::vec3(-0.1f, 0.1f, 0.1f)},
    Vertex{glm::vec3(-0.1f, 0.1f, -0.1f)},
    Vertex{glm::vec3(0.1f, 0.1f, -0.1f)},
    Vertex{glm::vec3(0.1f, 0.1f, 0.1f)},
};

GLuint lightIndices[] = {
    0, 1, 2,
    0, 2, 3,
    0, 4, 7,
    0, 7, 3,
    3, 7, 6,
    3, 6, 2,
    2, 6, 5,
    2, 5, 1,
    1, 5, 4,
    1, 4, 0,
    4, 5, 6,
    4, 6, 7};

glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f, 0.2f, 2.0f),
    glm::vec3(2.3f, -3.3f, -4.0f),
};

// Scene lights
const DirectionalLight dirLight{
    .direction = {-0.2f, -1.0f, -0.3f},
    .ambient = {0.05f, 0.05f, 0.2f},
    .diffuse = {0.25f, 0.25f, 1.0f},
    .specular = {0.5f, 0.5f, 0.5f},
};

const PointLight pointLights[] = {
    {
        .position = pointLightPositions[0],
        .ambient = {0.05f, 0.2f, 0.05f},
        .diffuse = {0.25f, 1.0f, 0.25f},
        .specular = {1.0f, 1.0f, 1.0f},
    },
    {
        .position = pointLightPositions[1],
        .ambient = {0.2f, 0.05f, 0.05f},
        .diffuse = {1.0f, 0.25f, 0.25f},
        .specular = {1.0f, 1.0f, 1.0f},
    },
};

// SpotLight position/direction are updated each frame from the camera
SpotLight spotLight{
    .position = {0.f, 0.f, 0.f},   // overwritten each frame from camera
    .direction = {0.f, 0.f, -1.f}, // overwritten each frame from camera
    .ambient = {0.0f, 0.0f, 0.0f},
    .diffuse = {1.0f, 1.0f, 1.0f},
    .specular = {1.0f, 1.0f, 1.0f},
    // cutOff / outerCutOff use default values (12.5° / 17.5°)
};

int main()
{
    Window window(SCR_WIDTH, SCR_HEIGHT, "ProutEngine");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
    glEnable(GL_STENCIL_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode

    // load models
    // -----------
    TextureManager textureManager;
    Model ourModel("resources/models/backpack/backpack.obj", textureManager);

    // build and compile our shader program
    // ------------------------------------
    Shader vertexShader("resources/shaders/vs_default.glsl", ShaderType::VERTEX);
    Shader fragmentShader("resources/shaders/fs_default.glsl", ShaderType::FRAGMENT);
    ShaderProgram shaderProgram(vertexShader, fragmentShader);
    std::vector<Vertex> verts(cubeVertices, cubeVertices + sizeof(cubeVertices) / sizeof(Vertex));
    std::vector<GLuint> ind(cubeIndices, cubeIndices + sizeof(cubeIndices) / sizeof(GLuint));
    std::vector<TextureRef> tex{
        {textureManager.load("resources/textures/container2.png"), "texture_diffuse"},
        {textureManager.load("resources/textures/container2_specular.png"), "texture_specular"},
    };
    Mesh cube(verts, ind, tex);
    Log::info("Shader program default created.");

    // light shader
    // ------------
    Shader vertexLightShader("resources/shaders/vs_light.glsl", ShaderType::VERTEX);
    Shader fragmentLightShader("resources/shaders/fs_light.glsl", ShaderType::FRAGMENT);
    ShaderProgram lightShader(vertexLightShader, fragmentLightShader);
    std::vector<Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
    std::vector<GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
    Mesh light(lightVerts, lightInd, {});
    Log::info("Shader program light created.");

    // light cube parametrization
    // --------------------------
    glm::vec3 lightColor = glm::vec3(1.0f);

    // shaders setup
    // -------------
    shaderProgram.use();
    shaderProgram.setFloat("material.shininess", 32.0f);
    LightUtils::uploadToShader(shaderProgram, dirLight);
    LightUtils::uploadToShader(shaderProgram, pointLights[0], 0);
    LightUtils::uploadToShader(shaderProgram, pointLights[1], 1);
    LightUtils::uploadToShader(shaderProgram, spotLight);
    Log::info("Shader program default setup.");

    lightShader.use();
    lightShader.setVec3("lightColor", lightColor);
    Log::info("Shader program light setup.");

    Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

    Timer timer;
    InputManager inputManager(window.handle());
    CameraController cameraController(camera, inputManager);

    // Discrete events — subscriptions
    auto escConn = inputManager
                       .key(GLFW_KEY_ESCAPE)
                       .pressed
                       .connect([&]
                                { glfwSetWindowShouldClose(window.handle(), true); });

    // render loop
    // -----------
    while (!window.shouldClose())
    {
        timer.update();
        inputManager.update();

        // input
        // -----
        cameraController.update(timer.deltaTime());

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        shaderProgram.use();
        // Update per-frame spotlight position/direction from camera
        spotLight.position = camera.position;
        spotLight.direction = camera.direction;
        shaderProgram.setVec3("spotLight.position", spotLight.position);
        shaderProgram.setVec3("spotLight.direction", spotLight.direction);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));     // it's a bit too big for our scene, so scale it down
        glm::mat3 normal = glm::transpose(glm::inverse(model));
        shaderProgram.setMat4("model", model);
        shaderProgram.setMat3("normal", normal);
        ourModel.draw(shaderProgram, camera);

        // // Affichage du cube à toutes les cubePositions
        // for (unsigned int i = 0; i < 10; i++)
        // {
        //     // * Ces calculs de matrices sont chers à faire sur le GPU, ont les fait donc sur le CPU
        //     glm::mat4 localCubeModel = glm::mat4(1.0f);
        //     localCubeModel = glm::translate(localCubeModel, cubePositions[i]);

        //     // Rotation de chaque cube à un angle différent
        //     float angle = 20.0f * i;
        //     localCubeModel = glm::rotate(localCubeModel, glm::radians(angle) + (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));

        //     glm::mat3 localCubeNormal = glm::transpose(glm::inverse(localCubeModel));

        //     shaderProgram.setMat4("model", localCubeModel);
        //     shaderProgram.setMat3("normal", localCubeNormal);

        //     cube.draw(shaderProgram, camera, textureManager);
        // }

        // lightShader.use();
        // for (unsigned int i = 0; i < 2; i++)
        // {
        //     glm::mat4 localLightModel = glm::mat4(1.0f);
        //     localLightModel = glm::translate(localLightModel, pointLightPositions[i]);
        //     localLightModel = glm::scale(localLightModel, glm::vec3(0.2f)); // Make it a smaller cube
        //     lightShader.setMat4("model", localLightModel);

        //     light.draw(lightShader, camera, textureManager);
        // }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        window.swapBuffers();
        Window::pollEvents();
    }

    return 0;
}
