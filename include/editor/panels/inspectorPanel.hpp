#pragma once

class Scene;

// ---------------------------------------------------------------------------
// InspectorPanel — "ProutEngine Debug" window: FPS, scene, camera, entities
// (with transform editing) and lights.
// ---------------------------------------------------------------------------
class InspectorPanel
{
public:
    void draw(Scene &scene);

private:
    int selectedEntity_ = -1;
};
