#include "core/window.hpp"

#include <iostream>
#include <stdexcept>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

Window::Window(unsigned int width, unsigned int height, const std::string &title)
    : window_{nullptr}, width_{width}, height_{height}
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window_ = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
    if (!window_)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window_);
    glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }
}

Window::~Window()
{
    glfwDestroyWindow(window_);
    glfwTerminate();
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(window_);
}

void Window::swapBuffers()
{
    // glfw works with 2 buffers, a front one (which is displayed) and a back one (which is being drawn to).
    // After drawing on the back buffer, we swap it with the front buffer to display it.
    glfwSwapBuffers(window_);
}

void Window::pollEvents()
{
    glfwPollEvents();
}

void Window::framebufferSizeCallback(GLFWwindow * /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}
