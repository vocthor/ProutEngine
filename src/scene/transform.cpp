#include "scene/transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Transform::getModelMatrix() const
{
    glm::mat4 m = glm::mat4(1.f);
    m = glm::translate(m, position);
    m = glm::rotate(m, glm::radians(rotation.x), {1, 0, 0});
    m = glm::rotate(m, glm::radians(rotation.y), {0, 1, 0});
    m = glm::rotate(m, glm::radians(rotation.z), {0, 0, 1});
    m = glm::scale(m, scale);
    return m;
}