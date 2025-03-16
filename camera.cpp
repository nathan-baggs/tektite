#include "camera.h"

#include "clib.h"
#include "log.h"
#include "matrix4.h"
#include "padding.h"
#include "vector3.h"

namespace
{

auto create_direction(float pitch, float yaw) -> Vector3
{
    PADDING_LINE_THREE_QUARTER;
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
    PADDING_LINE_THREE_QUARTER;
    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::look_at(position_, position_ + direction_, up_);
}

auto Camera::position() const -> Vector3
{
    PADDING_LINE_THREE_QUARTER;
    return position_;
}

auto Camera::direction() const -> Vector3
{
    PADDING_LINE_THREE_QUARTER;
    return direction_;
}

auto Camera::right() const -> Vector3
{
    PADDING_LINE_THREE_QUARTER;
    return Vector3::normalise(Vector3::cross(direction_, up_));
}

auto Camera::adjust_yaw(float adjust) -> void
{
    PADDING_LINE_THREE_QUARTER;
    yaw_ += adjust;
    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::look_at(position_, position_ + direction_, up_);
}

auto Camera::adjust_pitch([[maybe_unused]] float adjust) -> void
{
    PADDING_LINE_THREE_QUARTER;
    pitch_ += adjust;
    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::look_at(position_, position_ + direction_, up_);
}

auto Camera::translate(const Vector3 &translation) -> void
{
    PADDING_LINE_THREE_QUARTER;
    position_ += translation;
    direction_ = create_direction(pitch_, yaw_);
    view_ = Matrix4::look_at(position_, position_ + direction_, up_);
}

auto Camera::view() const -> const float *
{
    PADDING_LINE_THREE_QUARTER;
    return view_.data();
}

auto Camera::projection() const -> const float *
{
    PADDING_LINE_THREE_QUARTER;
    return projection_.data();
}
