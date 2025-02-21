

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include "shader.hpp"
#include "VBO.hpp"
#include "VAO.hpp"
#include "EBO.hpp"
#include "texture.hpp"
#include "camera.hpp"

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
GLfloat cubeVertices[] = {
    //   COORDINATES    /          COLORS          /    TEXTURE COORD    /       NORMAL       //
    // Front
    -0.5f, -0.5f, 0.5f, /**/ 1.0f, 0.0f, 0.0f, 1.0f, /**/ 0.0f, 0.0f, /**/ 0.0f, 0.0f, 1.0f, // Lower left front corner
    -0.5f, 0.5f, 0.5f, /**/ 0.0f, 0.0f, 1.0f, 1.0f, /**/ 0.0f, 1.0f, /**/ 0.0f, 0.0f, 1.0f,  // Upper left front corner
    0.5f, 0.5f, 0.5f, /**/ 0.0f, 1.0f, 0.0f, 1.0f, /**/ 1.0f, 1.0f, /**/ 0.0f, 0.0f, 1.0f,   // Upper right front corner
    0.5f, -0.5f, 0.5f, /**/ 1.0f, 1.0f, 0.0f, 1.0f, /**/ 1.0f, 0.0f, /**/ 0.0f, 0.0f, 1.0f,  // Lower right front corner
    // Back
    -0.5f, -0.5f, -0.5f, /**/ 1.0f, 0.0f, 0.0f, 1.0f, /**/ 1.0f, 0.0f, /**/ 0.0f, 0.0f, -1.0f, // Lower left back corner
    -0.5f, 0.5f, -0.5f, /**/ 0.0f, 0.0f, 1.0f, 1.0f, /**/ 1.0f, 1.0f, /**/ 0.0f, 0.0f, -1.0f,  // Upper left back corner
    0.5f, 0.5f, -0.5f, /**/ 0.0f, 1.0f, 0.0f, 1.0f, /**/ 0.0f, 1.0f, /**/ 0.0f, 0.0f, -1.0f,   // Upper right back corner
    0.5f, -0.5f, -0.5f, /**/ 1.0f, 1.0f, 0.0f, 1.0f, /**/ 0.0f, 0.0f, /**/ 0.0f, 0.0f, -1.0f,  // Lower right back corner
    // Lower
    -0.5f, -0.5f, 0.5f, /**/ 1.0f, 0.0f, 0.0f, 1.0f, /**/ 0.0f, 0.0f, /**/ 0.0f, -1.0f, 0.0f,  // Lower left front corner
    0.5f, -0.5f, 0.5f, /**/ 1.0f, 1.0f, 0.0f, 1.0f, /**/ 1.0f, 0.0f, /**/ 0.0f, -1.0f, 0.0f,   // Lower right front corner
    0.5f, -0.5f, -0.5f, /**/ 1.0f, 1.0f, 0.0f, 1.0f, /**/ 0.0f, 0.0f, /**/ 0.0f, -1.0f, 0.0f,  // Lower right back corner
    -0.5f, -0.5f, -0.5f, /**/ 1.0f, 0.0f, 0.0f, 1.0f, /**/ 1.0f, 0.0f, /**/ 0.0f, -1.0f, 0.0f, // Lower left back corner
    // Upper
    -0.5f, 0.5f, 0.5f, /**/ 0.0f, 0.0f, 1.0f, 1.0f, /**/ 0.0f, 1.0f, /**/ 0.0f, 1.0f, 0.0f,  // Upper left front corner
    0.5f, 0.5f, 0.5f, /**/ 0.0f, 1.0f, 0.0f, 1.0f, /**/ 1.0f, 1.0f, /**/ 0.0f, 1.0f, 0.0f,   // Upper right front corner
    0.5f, 0.5f, -0.5f, /**/ 0.0f, 1.0f, 0.0f, 1.0f, /**/ 0.0f, 1.0f, /**/ 0.0f, 1.0f, 0.0f,  // Upper right back corner
    -0.5f, 0.5f, -0.5f, /**/ 0.0f, 0.0f, 1.0f, 1.0f, /**/ 1.0f, 1.0f, /**/ 0.0f, 1.0f, 0.0f, // Upper left back corner
    // Right
    0.5f, 0.5f, 0.5f, /**/ 0.0f, 1.0f, 0.0f, 1.0f, /**/ 1.0f, 1.0f, /**/ 1.0f, 0.0f, 0.0f,   // Upper right front corner
    0.5f, -0.5f, 0.5f, /**/ 1.0f, 1.0f, 0.0f, 1.0f, /**/ 1.0f, 0.0f, /**/ 1.0f, 0.0f, 0.0f,  // Lower right front corner
    0.5f, -0.5f, -0.5f, /**/ 1.0f, 1.0f, 0.0f, 1.0f, /**/ 0.0f, 0.0f, /**/ 1.0f, 0.0f, 0.0f, // Lower right back corner
    0.5f, 0.5f, -0.5f, /**/ 0.0f, 1.0f, 0.0f, 1.0f, /**/ 0.0f, 1.0f, /**/ 1.0f, 0.0f, 0.0f,  // Upper right back corner
    // Left
    -0.5f, -0.5f, 0.5f, /**/ 1.0f, 0.0f, 0.0f, 1.0f, /**/ 0.0f, 0.0f, /**/ -1.0f, 0.0f, 0.0f,  // Lower left front corner
    -0.5f, 0.5f, 0.5f, /**/ 0.0f, 0.0f, 1.0f, 1.0f, /**/ 0.0f, 1.0f, /**/ -1.0f, 0.0f, 0.0f,   // Upper left front corner
    -0.5f, 0.5f, -0.5f, /**/ 0.0f, 0.0f, 1.0f, 1.0f, /**/ 1.0f, 1.0f, /**/ -1.0f, 0.0f, 0.0f,  // Upper left back corner
    -0.5f, -0.5f, -0.5f, /**/ 1.0f, 0.0f, 0.0f, 1.0f, /**/ 1.0f, 0.0f, /**/ -1.0f, 0.0f, 0.0f, // Lower left back corner
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
    glm::vec3(-1.3f, 1.0f, -1.5f)};

GLfloat lightVertices[] = {
    //     COORDINATES     //
    -0.1f, -0.1f, 0.1f,
    -0.1f, -0.1f, -0.1f,
    0.1f, -0.1f, -0.1f,
    0.1f, -0.1f, 0.1f,
    -0.1f, 0.1f, 0.1f,
    -0.1f, 0.1f, -0.1f,
    0.1f, 0.1f, -0.1f,
    0.1f, 0.1f, 0.1f};

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

    // build and compile our shader program
    // ------------------------------------
    Shader shaderProgram("resources/shaders/vs_default.glsl", "resources/shaders/fs_default.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    VAO VAO1;
    VAO1.bind();
    VBO VBO1(cubeVertices, sizeof(cubeVertices));
    EBO EBO1(cubeIndices, sizeof(cubeIndices));
    VAO1.linkAttrib(VBO1, 0, 3, GL_FLOAT, 12 * sizeof(float), (void *)0);
    VAO1.linkAttrib(VBO1, 1, 4, GL_FLOAT, 12 * sizeof(float), (void *)(3 * sizeof(float)));
    VAO1.linkAttrib(VBO1, 2, 2, GL_FLOAT, 12 * sizeof(float), (void *)(7 * sizeof(float)));
    VAO1.linkAttrib(VBO1, 3, 3, GL_FLOAT, 12 * sizeof(float), (void *)(9 * sizeof(float)));
    VAO1.unbind();
    VBO1.unbind();
    EBO1.unbind();

    // light shader
    // ------------
    Shader lightShader("resources/shaders/vs_light.glsl", "resources/shaders/fs_light.glsl");
    VAO lightVAO;
    lightVAO.bind();
    // ! TODO : replace lightVBO and lightEBO by VBO1 and EBO1 because all are cubes ... ?
    VBO lightVBO(lightVertices, sizeof(lightVertices));
    EBO lightEBO(lightIndices, sizeof(lightIndices));
    lightVAO.linkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void *)0);
    lightVAO.unbind();
    lightVBO.unbind();
    lightEBO.unbind();

    // light cube parametrization
    // --------------------------
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.f, 0.f, -1.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);

    // shaders setup
    // -------------
    shaderProgram.use();
    shaderProgram.setVec4("lightColor", lightColor);
    shaderProgram.setVec3("lightPos", lightPos);
    lightShader.use();
    shaderProgram.setMat4("model", lightModel);
    lightShader.setVec4("lightColor", lightColor);

    // textures
    // --------
    Texture popCat("resources/textures/pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    popCat.texUnit(shaderProgram, "texture0", 0);

    Texture container("resources/textures/container.jpg", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
    container.texUnit(shaderProgram, "texture1", 1);

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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the triangle
        shaderProgram.use();
        popCat.bind();
        container.bind();

        camera.matrix(50.0f, 0.1f, 100.0f, shaderProgram);
        shaderProgram.setVec3("viewPos", camera.position);

        VAO1.bind();

        // Affichage du cube à toutes les cubePositions
        for (unsigned int i = 0; i < 10; i++)
        {
            // * Ces calculs de matrices sont chers à faire sur le GPU, ont le sfait donc sur le CPU
            glm::mat4 localCubeModel = glm::mat4(1.0f);
            localCubeModel = glm::translate(localCubeModel, cubePositions[i]);

            // Rotation de chaque cube à un angle différent
            float angle = 20.0f * i;
            localCubeModel = glm::rotate(localCubeModel, glm::radians(angle) + (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));

            glm::mat3 localCubeNormal = glm::transpose(glm::inverse(localCubeModel));

            shaderProgram.setMat4("model", localCubeModel);
            shaderProgram.setMat3("normal", localCubeNormal);

            glDrawElements(GL_TRIANGLES, sizeof(cubeIndices) / sizeof(int), GL_UNSIGNED_INT, 0);
        }

        lightShader.use();
        camera.matrix(50.0f, 0.1f, 100.0f, lightShader);
        lightVAO.bind();
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    VAO1.remove();
    VBO1.remove();
    EBO1.remove();
    container.remove();

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
