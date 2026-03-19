#pragma once

#include <array>
#include <stdexcept>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "input/signal.hpp"

struct KeySignals
{
    Signal<> pressed;
    Signal<> released;
};

struct MouseSignals
{
    Signal<> pressed;
    Signal<> released;
};

class InputManager
{
public:
    explicit InputManager(GLFWwindow *window);
    ~InputManager();

    // Call once per frame: transfers accumulated mouse delta and resets it
    void update();

    // --- Poll interface (continuous state, direct GLFW query) ---
    bool isHeld(int key) const;
    bool isMouseHeld(int button) const;

    glm::vec2 mousePos() const { return mousePos_; }
    glm::vec2 mouseDelta() const { return mouseDelta_; }

    void setCursorMode(int mode);

    // --- Signal interface (fired from GLFW callbacks) ---
    // input.key(GLFW_KEY_A).pressed.connect(fn)
    KeySignals &key(int glfwKey);
    MouseSignals &mouseButton(int glfwButton);

    Signal<glm::vec2> onMouseMoved; // fired per movement event (delta)

private:
    static void keyCallback(GLFWwindow *, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow *, int button, int action, int mods);
    static void cursorCallback(GLFWwindow *, double x, double y);

    GLFWwindow *window_;

    std::array<KeySignals, GLFW_KEY_LAST + 1> keySignals_;
    std::array<MouseSignals, GLFW_MOUSE_BUTTON_LAST + 1> mouseSignals_;

    glm::vec2 mousePos_ = {0.f, 0.f};
    glm::vec2 mouseDelta_ = {0.f, 0.f};       // exposed after update()
    glm::vec2 accumulatedDelta_ = {0.f, 0.f}; // written by cursorCallback
    glm::vec2 prevMousePos_ = {0.f, 0.f};
    bool firstCursorEvent_ = true;
};
