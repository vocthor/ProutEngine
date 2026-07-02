#pragma once

struct GLFWwindow;
class Scene;

class ImGuiLayer
{
public:
    explicit ImGuiLayer(GLFWwindow *window);
    ~ImGuiLayer();

    ImGuiLayer(const ImGuiLayer &) = delete;
    ImGuiLayer &operator=(const ImGuiLayer &) = delete;
    ImGuiLayer(ImGuiLayer &&) = delete;
    ImGuiLayer &operator=(ImGuiLayer &&) = delete;

    void beginFrame();
    void renderDebugPanel(Scene &scene);
    void endFrame();

    bool wantsMouseCapture() const;
    bool wantsKeyboardCapture() const;

private:
    bool debugGuiEnabled_ = true;
    int selectedEntityIndex_ = -1;
};
