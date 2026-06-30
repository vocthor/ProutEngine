#include "core/timer.hpp"

#include <GLFW/glfw3.h>

void Timer::update()
{
    float now    = static_cast<float>(::glfwGetTime());
    deltaTime_   = now - lastFrame_;
    lastFrame_   = now;
}
