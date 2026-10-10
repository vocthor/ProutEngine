#pragma once

class Window;
class Timer;
class InputManager;
class Scene;
class Renderer;
class TextureManager;
class RenderTarget;

// ---------------------------------------------------------------------------
// EngineContext — services owned by Application and shared with every Layer.
// ---------------------------------------------------------------------------
struct EngineContext
{
    Window &window;
    Timer &timer;
    InputManager &inputManager;
    TextureManager &textureManager;
    /** The main scene containing all entities and lights. Needs to be shared between game and editor. */
    Scene &scene;
    Renderer &renderer;
    /** The render target where the scene will be drawn. Can be either the screen or an offscreen buffer. Never null. */
    RenderTarget *renderTarget;
    /** Set by the editor: false when the game must not react to input (e.g. ImGui owns the mouse). */
    bool gameInputEnabled = true;
};
