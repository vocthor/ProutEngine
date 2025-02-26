#ifndef CAMERA_H
#define CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader.hpp"

class Camera
{
public:
    // Stores the main vectors of the camera
    glm::vec3 position;
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Prevents the camera from jumping around when first clicking left click
    bool firstClick = true;

    // Stores the width and height of the window
    int width;
    int height;

    // Adjust the speed of the camera and it's sensitivity when looking around
    float speed = 0.5f;
    float sensitivity = 0.1f;

    // Camera constructor to set up initial values
    Camera(int width, int height, glm::vec3 pos);

    // Updates and exports the camera matrix to the Vertex Shader
    // ? TODO : séparer en 2 fonctions (cf tuto yt) ?
    void matrix(float FOVdeg, float nearPlane, float farPlane, Shader &shader);
    // Handles camera inputs
    void inputs(GLFWwindow *window, float deltaTime);

private:
    void processKeyboardEvent(GLFWwindow *window, float deltaTime);
    void processMouseEvent(GLFWwindow *window);
    float lastX;
    float lastY;
};
#endif