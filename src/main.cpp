

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
GLfloat vertices[] = {
    //   COORDINATES    /          COLORS          /    TEXTURE COORD  //
    -0.5f, -0.5f, 0.5f, /**/ 1.0f, 0.0f, 0.0f, 1.0f, /**/ 0.0f, 0.0f,  // Lower left corner
    -0.5f, 0.5f, 0.5f, /**/ 0.0f, 0.0f, 1.0f, 1.0f, /**/ 0.0f, 1.0f,   // Upper left corner
    0.5f, 0.5f, 0.5f, /**/ 0.0f, 1.0f, 0.0f, 1.0f, /**/ 1.0f, 1.0f,    // Upper right corner
    0.5f, -0.5f, 0.5f, /**/ 1.0f, 1.0f, 0.0f, 1.0f, /**/ 1.0f, 0.0f,   // Lower right corner
    -0.5f, -0.5f, -0.5f, /**/ 1.0f, 0.0f, 0.0f, 1.0f, /**/ 1.0f, 0.0f, // Lower left corner
    -0.5f, 0.5f, -0.5f, /**/ 0.0f, 0.0f, 1.0f, 1.0f, /**/ 1.0f, 1.0f,  // Upper left corner
    0.5f, 0.5f, -0.5f, /**/ 0.0f, 1.0f, 0.0f, 1.0f, /**/ 0.0f, 1.0f,   // Upper right corner
    0.5f, -0.5f, -0.5f, /**/ 1.0f, 1.0f, 0.0f, 1.0f, /**/ 0.0f, 0.0f,  // Lower right corner
};

// Indices for vertices order
GLuint indices[] = {
    0, 2, 1, // Upper-right triangle
    0, 3, 2, // Lower-left triangle
    0, 4, 7,
    0, 7, 3,
    0, 4, 5,
    0, 1, 5,
    6, 2, 1,
    6, 3, 2,
    6, 4, 7,
    6, 7, 3,
    6, 4, 5,
    6, 1, 5};

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
    Shader ourShader("resources/shaders/vs_default.glsl", "resources/shaders/fs_default.glsl"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    VAO VAO1;
    VAO1.bind();

    VBO VBO1(vertices, sizeof(vertices));
    EBO EBO1(indices, sizeof(indices));
    VAO1.linkAttrib(VBO1, 0, 3, GL_FLOAT, 9 * sizeof(float), (void *)0);
    VAO1.linkAttrib(VBO1, 1, 4, GL_FLOAT, 9 * sizeof(float), (void *)(3 * sizeof(float)));
    VAO1.linkAttrib(VBO1, 2, 2, GL_FLOAT, 9 * sizeof(float), (void *)(7 * sizeof(float)));
    VAO1.unbind();
    VBO1.unbind();
    EBO1.unbind();

    Texture popCat("resources/textures/pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    popCat.texUnit(ourShader, "texture0", 0);

    Texture container("resources/textures/container.jpg", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
    container.texUnit(ourShader, "texture1", 1);

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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render the triangle
        ourShader.use();
        popCat.bind();
        container.bind();

        camera.matrix(50.0f, 0.1f, 100.0f, ourShader);

        VAO1.bind();

        // Affichage du cube à toutes les cubePositions
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);

            // Rotation de chaque cube à un angle différent
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle) + (float)glfwGetTime(), glm::vec3(1.0f, 0.3f, 0.5f));

            ourShader.setMat4("model", model);

            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
        }

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
