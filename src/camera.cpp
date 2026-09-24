#include "camera.hpp"

Camera::Camera(int width, int height, glm::vec3 pos)
    : position{pos}, width{width}, height{height}
{
}

void Camera::matrix(float FOVdeg, float nearPlane, float farPlane, ShaderProgram &shaderProgram)
{
    glm::mat4 view = glm::lookAt(position, position + direction, up);
    glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), static_cast<float>(width) / height, nearPlane, farPlane);

    shaderProgram.setUniform<glm::mat4>("projection", projection);
    shaderProgram.setUniform<glm::mat4>("view", view);
}
