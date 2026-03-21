#pragma once

#include <glm/glm.hpp>

/**
 * @brief Represents a transform in 3D space, including position, rotation, and scale.
 *
 * Coordinate system: right-handed
 *   - X : right
 *   - Y : up
 *   - Z : backward (toward the viewer)
 *
 * Rotations are stored as Euler angles in degrees (rotation.x, rotation.y, rotation.z).
 * They map to the standard aeronautic conventions:
 *   - rotation.x : Pitch — rotation around the X axis (nose up/down)
 *   - rotation.y : Yaw   — rotation around the Y axis (nose left/right)
 *   - rotation.z : Roll  — rotation around the Z axis (tilt left/right)
 *
 * Application order in getModelMatrix(): Pitch (X) → Yaw (Y) → Roll (Z), applied
 * in local space (i.e., each rotation is relative to the already-rotated axes).
 * This is equivalent to the extrinsic order Z → Y → X in world space.
 *
 * Note: Euler angles are subject to gimbal lock. For smooth arbitrary rotations,
 * consider switching to quaternions in a future iteration.
 */
struct Transform
{
    glm::vec3 position{0.f, 0.f, 0.f};
    glm::vec3 rotation{0.f, 0.f, 0.f}; // Euler angles in degrees
    glm::vec3 scale{1.f, 1.f, 1.f};

    glm::mat4 getModelMatrix() const;

    glm::mat3 getNormalMatrix() const
    {
        return glm::transpose(glm::inverse(getModelMatrix()));
    }

    // TODO : caching matrix() return value
    // and only recomputing it when position/rotation/scale changes
};