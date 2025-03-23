#pragma once

#include "matrix4.h"
#include "vector3.h"

/**
 * Class representing a camera.
 */
class Camera
{
  public:
    /**
     * Construct a new camera.
     *
     * @param position
     *   The position of the camera.
     * @param look_at
     *   The point the camera is looking at.
     * @param up
     *   The up vector of the camera.
     * @param fov
     *   The field of view of the camera.
     * @param width
     *   The width of the camera.
     * @param height
     *   The height of the camera.
     * @param near_plane
     *   The near plane of the camera.
     * @param far_plane
     *   The far plane of the camera.
     */
    Camera(
        const Vector3 &position,
        const Vector3 &look_at,
        const Vector3 &up,
        float fov,
        float width,
        float height,
        float near_plane,
        float far_plane);

    /**
     * Get the position of the camera.
     *
     * @return
     *   The position of the camera.
     */
    auto position() const -> Vector3;

    /**
     * Get the direction of the camera.
     *
     * @return
     *   The direction of the camera.
     */
    auto direction() const -> Vector3;

    /**
     * Get the vector poiting to the right of the camera direction.
     *
     * @return
     *   Camera right vector.
     */
    auto right() const -> Vector3;

    /**
     * Adjust the yaw of the camera.
     *
     * @param adjust
     *   The amount to adjust the yaw by.
     */
    auto adjust_yaw(float adjust) -> void;

    /**
     * Adjust the pitch of the camera.
     *
     * @param adjust
     *   The amount to adjust the pitch by.
     */
    auto adjust_pitch(float adjust) -> void;

    /**
     * Translate the camera.
     *
     * @param translation
     *   The translation to apply to the camera.
     */
    auto translate(const Vector3 &translation) -> void;

    /**
     * Get the view matrix of the camera.
     *
     * @return
     *   The view matrix of the camera.
     */
    auto view() const -> const float *;

    /**
     * Get the projection matrix of the camera.
     *
     * @return
     *   The projection matrix of the camera.
     */
    auto projection() const -> const float *;

  private:
    /** View matrix of the camera. */
    Matrix4 view_;

    /** Projection matrix of the camera. */
    Matrix4 projection_;

    /** Position of the camera. */
    Vector3 position_;

    /** Direction of the camera. */
    Vector3 direction_;

    /** Up vector of the camera. */
    Vector3 up_;

    /** Pitch of the camera. */
    float pitch_;

    /** Yaw of the camera. */
    float yaw_;
};
