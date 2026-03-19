#include "input/inputManager.hpp"

InputManager::InputManager(GLFWwindow *window)
    : window_{window}
{
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, keyCallback);
    glfwSetMouseButtonCallback(window_, mouseCallback);
    glfwSetCursorPosCallback(window_, cursorCallback);
}

InputManager::~InputManager()
{
    glfwSetKeyCallback(window_, nullptr);
    glfwSetMouseButtonCallback(window_, nullptr);
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetWindowUserPointer(window_, nullptr);
}

void InputManager::update()
{
    // Transfer delta accumulated by cursorCallback since last frame, then reset
    mouseDelta_ = accumulatedDelta_;
    accumulatedDelta_ = {0.f, 0.f};
}

bool InputManager::isHeld(int key) const
{
    if (key < 0 || key > GLFW_KEY_LAST)
        return false;
    return glfwGetKey(window_, key) == GLFW_PRESS;
}

bool InputManager::isMouseHeld(int button) const
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;
    return glfwGetMouseButton(window_, button) == GLFW_PRESS;
}

void InputManager::setCursorMode(int mode)
{
    glfwSetInputMode(window_, GLFW_CURSOR, mode);
}

KeySignals &InputManager::key(int glfwKey)
{
    if (glfwKey < 0 || glfwKey > GLFW_KEY_LAST)
        throw std::out_of_range("InputManager::key: invalid GLFW key");
    return keySignals_[glfwKey];
}

MouseSignals &InputManager::mouseButton(int glfwButton)
{
    if (glfwButton < 0 || glfwButton > GLFW_MOUSE_BUTTON_LAST)
        throw std::out_of_range("InputManager::mouseButton: invalid GLFW button");
    return mouseSignals_[glfwButton];
}

void InputManager::keyCallback(GLFWwindow *w, int key, int /*scancode*/, int action, int /*mods*/)
{
    auto *self = static_cast<InputManager *>(glfwGetWindowUserPointer(w));
    if (key < 0 || key > GLFW_KEY_LAST)
        return;
    if (action == GLFW_PRESS)
        self->keySignals_[key].pressed.emit();
    if (action == GLFW_RELEASE)
        self->keySignals_[key].released.emit();
    // GLFW_REPEAT ignored: use isHeld() for continuous input
}

void InputManager::mouseCallback(GLFWwindow *w, int button, int action, int /*mods*/)
{
    auto *self = static_cast<InputManager *>(glfwGetWindowUserPointer(w));
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return;
    if (action == GLFW_PRESS)
        self->mouseSignals_[button].pressed.emit();
    if (action == GLFW_RELEASE)
        self->mouseSignals_[button].released.emit();
}

void InputManager::cursorCallback(GLFWwindow *w, double x, double y)
{
    auto *self = static_cast<InputManager *>(glfwGetWindowUserPointer(w));
    glm::vec2 newPos = {static_cast<float>(x), static_cast<float>(y)};
    self->mousePos_ = newPos;
    if (!self->firstCursorEvent_)
    {
        glm::vec2 delta = newPos - self->prevMousePos_;
        self->accumulatedDelta_ += delta;
        self->onMouseMoved.emit(delta);
    }
    self->prevMousePos_ = newPos;
    self->firstCursorEvent_ = false;
}
