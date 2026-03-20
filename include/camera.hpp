#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaderProgram.hpp"

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    int width;
    int height;

    float sensitivity = 0.1f;

    Camera(int width, int height, glm::vec3 pos);

    void matrix(float FOVdeg, float nearPlane, float farPlane, ShaderProgram &shaderProgram);
};