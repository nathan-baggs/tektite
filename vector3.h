#pragma once

#include <cmath>

struct Vector3
{
    constexpr Vector3()
        : Vector3(0.0f)
    {
    }

    constexpr Vector3(float xyz)
        : Vector3(xyz, xyz, xyz)
    {
    }

    constexpr Vector3(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    static auto normalise(const Vector3 &v) -> Vector3
    {
        const auto length = std::hypot(v.x, v.y, v.z);
        if (length == 0.0f)
        {
            return {};
        }
        return {v.x / length, v.y / length, v.z / length};
    }

    static constexpr auto cross(const Vector3 &v1, const Vector3 &v2) -> Vector3
    {
        const auto i = (v1.y * v2.z) - (v1.z * v2.y);
        const auto j = (v1.x * v2.z) - (v1.z * v2.x);
        const auto k = (v1.x * v2.y) - (v1.y * v2.x);

        return {i, -j, k};
    }

    constexpr auto operator==(const Vector3 &) const -> bool = default;

    float x;
    float y;
    float z;
};

constexpr auto operator-=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;

    return v1;
}

constexpr auto operator-(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    auto tmp = v1;
    return tmp -= v2;
}

constexpr auto operator+=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;

    return v1;
}

constexpr auto operator+(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    auto tmp = v1;
    return tmp += v2;
}

constexpr auto operator*=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;

    return v1;
}

constexpr auto operator*(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    auto tmp = v1;
    return tmp *= v2;
}

constexpr auto operator-(const Vector3 &v) -> Vector3
{
    return {-v.x, -v.y, -v.z};
}
