#include "cameraController.hpp"

#include <cmath>

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

CameraController::CameraController(Camera &camera, InputManager &input)
    : camera_{camera}, input_{input}
{
}

void CameraController::update(float deltaTime)
{
    processKeyboard(deltaTime);
    processMouseLook();
}

void CameraController::processKeyboard(float deltaTime)
{
    float speed = input_.isHeld(GLFW_KEY_LEFT_SHIFT) ? sprintSpeed_ : baseSpeed_;
    float velocity = speed * deltaTime;

    if (input_.isHeld(GLFW_KEY_W))
        camera_.position += velocity * camera_.direction;
    if (input_.isHeld(GLFW_KEY_S))
        camera_.position -= velocity * camera_.direction;
    if (input_.isHeld(GLFW_KEY_A))
        camera_.position -= velocity * glm::normalize(glm::cross(camera_.direction, camera_.up));
    if (input_.isHeld(GLFW_KEY_D))
        camera_.position += velocity * glm::normalize(glm::cross(camera_.direction, camera_.up));
    if (input_.isHeld(GLFW_KEY_SPACE))
        camera_.position += velocity * camera_.up;
    if (input_.isHeld(GLFW_KEY_LEFT_CONTROL))
        camera_.position -= velocity * camera_.up;
}

void CameraController::processMouseLook()
{
    if (input_.isMouseHeld(GLFW_MOUSE_BUTTON_LEFT))
    {
        input_.setCursorMode(GLFW_CURSOR_HIDDEN);

        glm::vec2 delta = input_.mouseDelta();

        // Skip if we just started looking (avoids initial jump)
        if (!looking_)
        {
            looking_ = true;
            return;
        }

        float rotX = camera_.sensitivity * (-delta.y); // reversed: y goes top-to-bottom
        float rotY = camera_.sensitivity * delta.x;

        // Vertical rotation (pitch) — clamp to avoid flipping
        glm::vec3 right = glm::normalize(glm::cross(camera_.direction, camera_.up));
        glm::vec3 newDirection = glm::rotate(camera_.direction, glm::radians(rotX), right);

        if (std::abs(glm::angle(newDirection, camera_.up) - glm::radians(90.0f)) <= glm::radians(85.0f))
            camera_.direction = newDirection;

        // Horizontal rotation (yaw)
        camera_.direction = glm::rotate(camera_.direction, glm::radians(-rotY), camera_.up);
    }
    else
    {
        if (looking_)
        {
            input_.setCursorMode(GLFW_CURSOR_NORMAL);
            looking_ = false;
        }
    }
}
