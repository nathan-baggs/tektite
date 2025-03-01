#pragma once

#include "matrix4.h"
#include "vector3.h"

class Camera
{
  public:
    Camera(
        const Vector3 &position,
        const Vector3 &look_at,
        const Vector3 &up,
        float fov,
        float width,
        float height,
        float near_plane,
        float far_plane);

    auto position() const -> Vector3;
    auto direction() const -> Vector3;
    auto right() const -> Vector3;
    auto adjust_yaw(float adjust) -> void;
    auto adjust_pitch(float adjust) -> void;
    auto translate(const Vector3 &translation) -> void;
    auto view() const -> const float *;
    auto projection() const -> const float *;

  private:
    Matrix4 view_;
    Matrix4 projection_;
    Vector3 position_;
    Vector3 direction_;
    Vector3 up_;
    float pitch_;
    float yaw_;
};
