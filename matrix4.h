#pragma once

#include <array>

#include "clib.h"
#include "quaternion.h"
#include "vector3.h"

class Matrix4
{
  public:
    struct Scale
    {
    };

    constexpr Matrix4()
        : elements_({
              1.0f,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              1.0f,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              1.0f,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              1.0f,
          })
    {
    }

    constexpr Matrix4(const std::array<float, 16u> &elements)
        : elements_(elements)
    {
    }

    Matrix4(const float *elements)
    {
        for (auto i = 0u; i < 16u; ++i)
        {
            elements_[i] = elements[i];
        }
    }

    constexpr Matrix4(const Vector3 &translation)
        : elements_({
              1.0f,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              1.0f,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              1.0f,
              0.0f,
              translation.x,
              translation.y,
              translation.z,
              1.0f,
          })
    {
    }

    constexpr Matrix4(const Vector3 &scale, Scale)
        : elements_({
              scale.x,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              scale.y,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              scale.z,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              1.0f,
          })
    {
    }

    constexpr Matrix4(const Vector3 &translation, const Vector3 &scale)
        : elements_({
              scale.x,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              scale.y,
              0.0f,
              0.0f,
              0.0f,
              0.0f,
              scale.z,
              0.0f,
              translation.x,
              translation.y,
              translation.z,
              1.0f,
          })
    {
    }

    constexpr Matrix4(const Quaternion &rotation)
        : Matrix4{}
    {
        elements_[0] = 1.0f - 2.0f * rotation.y * rotation.y - 2.0f * rotation.z * rotation.z;
        elements_[1] = 2.0f * rotation.x * rotation.y + 2.0f * rotation.z * rotation.w;
        elements_[2] = 2.0f * rotation.x * rotation.z - 2.0f * rotation.y * rotation.w;

        elements_[4] = 2.0f * rotation.x * rotation.y - 2.0f * rotation.z * rotation.w;
        elements_[5] = 1.0f - 2.0f * rotation.x * rotation.x - 2.0f * rotation.z * rotation.z;
        elements_[6] = 2.0f * rotation.y * rotation.z + 2.0f * rotation.x * rotation.w;

        elements_[8] = 2.0f * rotation.x * rotation.z + 2.0f * rotation.y * rotation.w;
        elements_[9] = 2.0f * rotation.y * rotation.z - 2.0f * rotation.x * rotation.w;
        elements_[10] = 1.0f - 2.0f * rotation.x * rotation.x - 2.0f * rotation.y * rotation.y;
    }

    static auto look_at(const Vector3 &eye, const Vector3 &look_at, const Vector3 &up) -> Matrix4;
    static auto perspective(float fov, float width, float height, float near_plane, float far_plane) -> Matrix4;

    constexpr auto data() const -> const float *
    {
        return elements_.data();
    }

    constexpr auto operator[](std::size_t index) const -> const auto &
    {
        return elements_[index];
    }

    constexpr auto operator[](std::size_t index) -> auto &
    {
        return elements_[index];
    }

    friend constexpr auto operator*=(Matrix4 &m1, const Matrix4 &m2) -> Matrix4 &;

    constexpr auto operator==(const Matrix4 &) const -> bool = default;

  private:
    std::array<float, 16u> elements_;
};

constexpr auto operator*=(Matrix4 &m1, const Matrix4 &m2) -> Matrix4 &
{
    auto result = Matrix4{};
    for (auto i = 0u; i < 4u; ++i)
    {
        for (auto j = 0u; j < 4u; ++j)
        {
            result.elements_[i + j * 4] = 0.0f;
            for (auto k = 0u; k < 4u; ++k)
            {
                result.elements_[i + j * 4] += m1.elements_[i + k * 4] * m2.elements_[k + j * 4];
            }
        }
    }

    m1 = result;
    return m1;
}

constexpr auto operator*(const Matrix4 &m1, const Matrix4 &m2) -> Matrix4
{
    auto tmp{m1};
    return tmp *= m2;
}

inline auto Matrix4::look_at(const Vector3 &eye, const Vector3 &look_at, const Vector3 &up) -> Matrix4
{
    const auto f = Vector3::normalise(look_at - eye);
    const auto up_normalised = Vector3::normalise(up);

    const auto s = Vector3::normalise(Vector3::cross(f, up_normalised));
    const auto u = Vector3::normalise(Vector3::cross(s, f));

    auto m = Matrix4{};
    m.elements_ = {{s.x, u.x, -f.x, 0.0f, s.y, u.y, -f.y, 0.0f, s.z, u.z, -f.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};

    return m * Matrix4{-eye};
}

inline auto Matrix4::perspective(float fov, float width, float height, float near_plane, float far_plane) -> Matrix4
{
    Matrix4 m;

    const auto aspect_ratio = width / height;
    const auto tmp = tan(fov / 2.0f);
    const auto t = tmp * near_plane;
    const auto b = -t;
    const auto r = t * aspect_ratio;
    const auto l = b * aspect_ratio;

    m.elements_ = {
        {(2.0f * near_plane) / (r - l),
         0.0f,
         0.0f,
         0.0f,
         0.0f,
         (2.0f * near_plane) / (t - b),
         0.0f,
         0.0f,
         (r + l) / (r - l),
         (t + b) / (t - b),
         -(far_plane + near_plane) / (far_plane - near_plane),
         -1.0f,
         0.0f,
         0.0f,
         -(2.0f * far_plane * near_plane) / (far_plane - near_plane),
         0.0f}};
    return m;
}
