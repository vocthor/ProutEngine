#include "render/cameraUBO.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace
{
    constexpr ::GLintptr VIEW_OFFSET = 0;
    constexpr ::GLintptr PROJECTION_OFFSET = sizeof(glm::mat4);
    constexpr ::GLintptr VIEW_POS_OFFSET = sizeof(glm::mat4) * 2;
    constexpr ::GLsizeiptr CAMERA_UBO_SIZE = sizeof(glm::mat4) * 2 + sizeof(glm::vec4);
    constexpr ::GLuint CAMERA_BINDING_POINT = 0;
}

CameraUBO::CameraUBO()
    : ubo_{CAMERA_UBO_SIZE, GL_DYNAMIC_DRAW}
{
    ::glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING_POINT, ubo_.handle());
    ubo_.unbind();
}

void CameraUBO::update(const Camera &camera, float fovDeg, float nearPlane, float farPlane)
{
    const glm::mat4 view = glm::lookAt(camera.position, camera.position + camera.direction, camera.up);
    const glm::mat4 projection = glm::perspective(glm::radians(fovDeg),
                                                  static_cast<float>(camera.width) / static_cast<float>(camera.height),
                                                  nearPlane,
                                                  farPlane);
    const glm::vec4 viewPos = glm::vec4(camera.position, 1.0f);

    ubo_.bind();
    ::glBufferSubData(GL_UNIFORM_BUFFER, VIEW_OFFSET, sizeof(glm::mat4), glm::value_ptr(view));
    ::glBufferSubData(GL_UNIFORM_BUFFER, PROJECTION_OFFSET, sizeof(glm::mat4), glm::value_ptr(projection));
    ::glBufferSubData(GL_UNIFORM_BUFFER, VIEW_POS_OFFSET, sizeof(glm::vec4), glm::value_ptr(viewPos));
    ubo_.unbind();
}