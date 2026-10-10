#pragma once

struct GLFWwindow;

// ---------------------------------------------------------------------------
// ImGuiBackend — owns the ImGui context and its GLFW/OpenGL backends.
//
// Holds the global ImGui configuration and the per-frame begin/end calls.
// It draws nothing itself: panels are drawn between beginFrame() and endFrame().
// ---------------------------------------------------------------------------
class ImGuiBackend
{
public:
    explicit ImGuiBackend(GLFWwindow *window);
    ~ImGuiBackend();

    ImGuiBackend(const ImGuiBackend &) = delete;
    ImGuiBackend &operator=(const ImGuiBackend &) = delete;
    ImGuiBackend(ImGuiBackend &&) = delete;
    ImGuiBackend &operator=(ImGuiBackend &&) = delete;

    void beginFrame();
    void endFrame();

    bool wantsMouseCapture() const;
    bool wantsKeyboardCapture() const;
};
