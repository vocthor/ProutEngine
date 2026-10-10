#include "editor/imguiBackend.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

ImGuiBackend::ImGuiBackend(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ::ImGui::CreateContext();
    ::ImGui::StyleColorsDark();
    // Windows are moved by their title bar only, so dragging inside never moves them
    ::ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
    ::ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ::ImGui_ImplGlfw_InitForOpenGL(window, false);
    ::ImGui_ImplOpenGL3_Init("#version 330");
}

ImGuiBackend::~ImGuiBackend()
{
    ::ImGui_ImplOpenGL3_Shutdown();
    ::ImGui_ImplGlfw_Shutdown();
    ::ImGui::DestroyContext();
}

void ImGuiBackend::beginFrame()
{
    ::ImGui_ImplOpenGL3_NewFrame();
    ::ImGui_ImplGlfw_NewFrame();
    ::ImGui::NewFrame();
}

bool ImGuiBackend::wantsMouseCapture() const
{
    return ::ImGui::GetIO().WantCaptureMouse;
}

bool ImGuiBackend::wantsKeyboardCapture() const
{
    return ::ImGui::GetIO().WantCaptureKeyboard;
}

void ImGuiBackend::endFrame()
{
    ::ImGui::Render();
    ::ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());
}
