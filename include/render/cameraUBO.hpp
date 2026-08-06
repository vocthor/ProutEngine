#pragma once

#include "camera.hpp"
#include "render/gpuBuffer.hpp"

class CameraUBO
{
public:
    CameraUBO();

    void update(const Camera &camera, float fovDeg, float nearPlane, float farPlane);

private:
    UBO ubo_;
};