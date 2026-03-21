#pragma once

#include <vector>

#include "camera.hpp"
#include "scene/entity.hpp"
#include "scene/light.hpp"

class ShaderProgram;

class Scene
{
public:
    Camera camera;

    std::vector<Entity> entities;

    DirectionalLight directionalLight{};
    std::vector<PointLight> pointLights;
    SpotLight spotLight{};

    explicit Scene(Camera cam) : camera(cam) {}

    void uploadLights(ShaderProgram &shader) const;
};