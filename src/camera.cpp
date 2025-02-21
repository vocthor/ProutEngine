#include "camera.hpp"
#include <iostream>

Camera::Camera(int width, int height, glm::vec3 pos)
{
    Camera::width = width;
    Camera::height = height;
    position = pos;
    lastX = width / 2;
    lastY = height / 2;
}

void Camera::matrix(float FOVdeg, float nearPlane, float farPlane, Shader &shader)
{
    // Initializes matrices since otherwise they will be the null matrix
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    // glm::mat4 model = glm::mat4(1.0f);

    // Makes camera look in the right direction from the right position
    view = glm::lookAt(position, position + direction, up);
    // Adds perspective to the scene
    projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
    // model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

    // Exports the camera matrix to the Vertex Shader
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    // shader.setMat4("model", model);
    // glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::inputs(GLFWwindow *window, float deltaTime)
{
    processKeyboardEvent(window, deltaTime);
    processMouseEvent(window);
}

void Camera::processKeyboardEvent(GLFWwindow *window, float deltaTime)
{
    // Handles key inputs
    float velocity = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        position += velocity * direction;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        position += velocity * -glm::normalize(glm::cross(direction, up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        position += velocity * -direction;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        position += velocity * glm::normalize(glm::cross(direction, up));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        position += velocity * up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        position += velocity * -up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        speed = 2.f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
    {
        speed = 0.5f;
    }
}

void Camera::processMouseEvent(GLFWwindow *window)
{
    // Handles mouse inputs
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        // Hides mouse cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        // Stores the coordinates of the cursor
        double mouseX;
        double mouseY;
        // Fetches the coordinates of the cursor
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float xpos = static_cast<float>(mouseX);
        float ypos = static_cast<float>(mouseY);

        // Prevents camera from jumping on the first click
        if (firstClick)
        {
            lastX = xpos;
            lastY = ypos;
            // glfwSetCursorPos(window, (width / 2), (height / 2));
            firstClick = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
        lastX = xpos;
        lastY = ypos;

        // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
        // and then "transforms" them into degrees
        float rotX = sensitivity * yoffset;
        float rotY = sensitivity * xoffset;

        // Calculates upcoming vertical change in the direction
        glm::vec3 newdirection = glm::rotate(direction, glm::radians(rotX), glm::normalize(glm::cross(direction, up)));

        // Decides whether or not the next vertical direction is legal or not
        if (abs(glm::angle(newdirection, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            direction = newdirection;
        }

        // Rotates the direction left and right
        direction = glm::rotate(direction, glm::radians(-rotY), up);

        // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        // Unhides cursor since camera is not looking around anymore
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Makes sure the next time the camera looks around it doesn't jump
        firstClick = true;
    }
}
