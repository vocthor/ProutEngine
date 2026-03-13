#pragma once

#include <string>

struct GLFWwindow;

class Window
{
public:
    Window(unsigned int width, unsigned int height, const std::string &title);
    ~Window();

    // Non-copyable, non-movable (GLFW callbacks depend on a stable pointer)
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    Window(Window &&) = delete;
    Window &operator=(Window &&) = delete;

    bool shouldClose() const;
    void swapBuffers();

    GLFWwindow *handle() const { return window_; }
    unsigned int width() const { return width_; }
    unsigned int height() const { return height_; }

    static void pollEvents();

private:
    GLFWwindow *window_;
    unsigned int width_;
    unsigned int height_;

    static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
};
