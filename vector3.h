#pragma once

#include "clib.h"
#include "func.h"
#include "padding.h"

struct Vector3
{
    Vector3()
        : Vector3(0.0f)
    {
        PADDING_LINE;
    }

    Vector3(float xyz)
        : Vector3(xyz, xyz, xyz)
    {
        PADDING_LINE;
    }

    constexpr Vector3(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    static auto normalise(const Vector3 &v) -> Vector3;

    static auto cross(const Vector3 &v1, const Vector3 &v2) -> Vector3;

    static auto distance(const Vector3 &v1, const Vector3 &v2) -> float
    {
        PADDING_LINE;
        return hypot(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    auto operator==(const Vector3 &) const -> bool = default;

    float x;
    float y;
    float z;
};

inline auto operator-=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
{
    PADDING_LINE;
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;

    return v1;
}

inline auto operator-(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    PADDING_LINE;
    auto tmp = v1;
    return tmp -= v2;
}

inline auto operator+=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
{
    PADDING_LINE;
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;

    return v1;
}

inline auto operator+(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    PADDING_LINE;
    auto tmp = v1;
    return tmp += v2;
}

inline auto operator*=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
{
    PADDING_LINE;
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;

    return v1;
}

inline auto operator*(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    PADDING_LINE;
    auto tmp = v1;
    return tmp *= v2;
}

inline auto operator-(const Vector3 &v) -> Vector3
{
    PADDING_LINE;
    return {-v.x, -v.y, -v.z};
}
