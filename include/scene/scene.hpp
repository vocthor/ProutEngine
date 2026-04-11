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
    glm::vec3 ambientColor = glm::vec3(0.03f);

    explicit Scene(Camera cam) : camera(cam) {}

    void uploadLights(ShaderProgram &shader) const;
};