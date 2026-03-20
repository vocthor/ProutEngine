#pragma once

#include "camera.hpp"
#include "input/inputManager.hpp"

class CameraController
{
public:
    CameraController(Camera &camera, InputManager &input);

    void update(float deltaTime);

private:
    void processKeyboard(float deltaTime);
    void processMouseLook();

    Camera &camera_;
    InputManager &input_;

    bool looking_ = false;
    float baseSpeed_ = 0.5f;
    float sprintSpeed_ = 2.f;
};
