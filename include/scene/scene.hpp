#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "camera.hpp"
#include "scene/entity.hpp"
#include "scene/light.hpp"

class ShaderProgram;

class Scene
{
public:
    Camera camera;

    std::vector<Entity> entities;

    std::vector<Light> lights;
    glm::vec4 ambientColor{0.1f, 0.1f, 0.1f, 1.0f};

    explicit Scene(Camera cam) : camera(cam) {}

    void uploadLights(ShaderProgram &shader) const;
};