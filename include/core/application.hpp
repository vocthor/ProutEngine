#pragma once

#include "core/timer.hpp"
#include "core/window.hpp"

// ---------------------------------------------------------------------------
// Application — top-level owner of the engine lifecycle.
//
// Responsible for:
//   • Creating the window and initialising the OpenGL context (constructor)
//   • Loading all resources and running the main loop (run())
//
// Usage:
//   int main() { Application{}.run(); }
// ---------------------------------------------------------------------------
class Application
{
public:
    Application();
    void run();

private:
    Window window_;
    Timer  timer_;
};
