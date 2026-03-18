#pragma once

#include <GLFW/glfw3.h>

class Timer
{
public:
    void update()
    {
        float now = static_cast<float>(glfwGetTime());
        deltaTime_ = now - lastFrame_;
        lastFrame_ = now;
    }

    float deltaTime() const { return deltaTime_; }
    float elapsed() const { return lastFrame_; }

private:
    float deltaTime_ = 0.0f;
    float lastFrame_ = 0.0f;
};
