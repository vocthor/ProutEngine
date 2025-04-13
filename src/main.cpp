

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include "mesh.hpp"
#include "model.hpp"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

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

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ProutEngine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))
    glEnable(GL_STENCIL_TEST);

    // load models
    // -----------
    Model ourModel("resources/models/monstre/Monstre.obj");

    // textures
    // --------
    Texture textures[]{
        Texture("resources/textures/container2.png", "texture_diffuse"),
        Texture("resources/textures/container2_specular.png", "texture_specular"),
    };

    // build and compile our shader program
    // ------------------------------------
    Shader shaderProgram("resources/shaders/vs_default.glsl", "resources/shaders/fs_default.glsl");
    std::vector<Vertex> verts(cubeVertices, cubeVertices + sizeof(cubeVertices) / sizeof(Vertex));
    std::vector<GLuint> ind(cubeIndices, cubeIndices + sizeof(cubeIndices) / sizeof(GLuint));
    std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
    Mesh cube(verts, ind, tex);

    // light shader
    // ------------
    Shader lightShader("resources/shaders/vs_light.glsl", "resources/shaders/fs_light.glsl");
    std::vector<Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
    std::vector<GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));
    Mesh light(lightVerts, lightInd, {});

    // light cube parametrization
    // --------------------------
    glm::vec3 lightColor = glm::vec3(1.0f);

    // shaders setup
    // -------------
    shaderProgram.use();
    shaderProgram.setFloat("material.shininess", 32.0f);
    // directional light
    shaderProgram.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shaderProgram.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.2f);
    shaderProgram.setVec3("dirLight.diffuse", 0.25f, 0.25f, 1.0f);
    shaderProgram.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    shaderProgram.setVec3("pointLights[0].position", pointLightPositions[0]);
    shaderProgram.setVec3("pointLights[0].ambient", 0.05f, 0.2f, 0.05f);
    shaderProgram.setVec3("pointLights[0].diffuse", 0.25f, 1.0f, 0.25f);
    shaderProgram.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    shaderProgram.setFloat("pointLights[0].constant", 1.0f);
    shaderProgram.setFloat("pointLights[0].linear", 0.09f);
    shaderProgram.setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    shaderProgram.setVec3("pointLights[1].position", pointLightPositions[1]);
    shaderProgram.setVec3("pointLights[1].ambient", 0.2f, 0.05f, 0.05f);
    shaderProgram.setVec3("pointLights[1].diffuse", 1.0f, 0.25f, 0.25f);
    shaderProgram.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    shaderProgram.setFloat("pointLights[1].constant", 1.0f);
    shaderProgram.setFloat("pointLights[1].linear", 0.09f);
    shaderProgram.setFloat("pointLights[1].quadratic", 0.032f);
    // spotLight
    shaderProgram.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shaderProgram.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    shaderProgram.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    shaderProgram.setFloat("spotLight.constant", 1.0f);
    shaderProgram.setFloat("spotLight.linear", 0.09f);
    shaderProgram.setFloat("spotLight.quadratic", 0.032f);
    shaderProgram.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shaderProgram.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

    lightShader.use();
    lightShader.setVec3("lightColor", lightColor);

    Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        //! fusionner
        processInput(window);
        camera.inputs(window, deltaTime);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        shaderProgram.use();
        shaderProgram.setVec3("spotLight.position", camera.position);
        shaderProgram.setVec3("spotLight.direction", camera.direction);

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

        //     cube.draw(shaderProgram, camera);
        // }

        // lightShader.use();
        // for (unsigned int i = 0; i < 2; i++)
        // {
        //     glm::mat4 localLightModel = glm::mat4(1.0f);
        //     localLightModel = glm::translate(localLightModel, pointLightPositions[i]);
        //     localLightModel = glm::scale(localLightModel, glm::vec3(0.2f)); // Make it a smaller cube
        //     lightShader.setMat4("model", localLightModel);

        //     light.draw(lightShader, camera);
        // }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    cube.remove();
    light.remove();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
