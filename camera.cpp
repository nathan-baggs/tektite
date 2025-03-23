#include "camera.h"

#include "clib.h"
#include "log.h"
#include "matrix4.h"
#include "vector3.h"

namespace
{

/**
 * Helper function to create a direction vector from pitch and yaw.
 *
 * @param pitch
 *   The pitch of the camera.
 *
 * @param yaw
 *   The yaw of the camera.
 *
 * @return
 *   The direction vector.
 */
auto create_direction(float pitch, float yaw) -> Vector3
{
    return Vector3::normalise({cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch)});
}

}

Camera::Camera(
    const Vector3 &position,
    const Vector3 &look_at,
    const Vector3 &up,
    float fov,
    float width,
    float height,
    float near_plane,
    float far_plane)
    : view_(Matrix4::look_at(position, look_at, up))
    , projection_(Matrix4::perspective(fov, width, height, near_plane, far_plane))
    , position_(position)
    , direction_(look_at)
    , up_(up)
    , pitch_{}
    , yaw_{-M_PI / 2.0f}
{
    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::look_at(position_, position_ + direction_, up_);
}

auto Camera::position() const -> Vector3
{
    return position_;
}

auto Camera::direction() const -> Vector3
{
    return direction_;
}

auto Camera::right() const -> Vector3
{
    return Vector3::normalise(Vector3::cross(direction_, up_));
}

auto Camera::adjust_yaw(float adjust) -> void
{
    yaw_ += adjust;
    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::look_at(position_, position_ + direction_, up_);
}

auto Camera::adjust_pitch([[maybe_unused]] float adjust) -> void
{
    pitch_ += adjust;
    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::look_at(position_, position_ + direction_, up_);
}

auto Camera::translate(const Vector3 &translation) -> void
{
    position_ += translation;
    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::look_at(position_, position_ + direction_, up_);
}

auto Camera::view() const -> const float *
{
    return view_.data();
}

auto Camera::projection() const -> const float *
{
    return projection_.data();
}
