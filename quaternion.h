#pragma once

#include "clib.h"

class Quaternion
{
  public:
    constexpr Quaternion()
        : Quaternion(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    constexpr Quaternion(float x, float y, float z, float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    constexpr auto operator*=(const Quaternion &quaternion) -> Quaternion &
    {
        const Quaternion copy{*this};

        x = copy.w * quaternion.x + copy.x * quaternion.w + copy.y * quaternion.z - copy.z * quaternion.y;
        y = copy.w * quaternion.y + copy.y * quaternion.w + copy.z * quaternion.x - copy.x * quaternion.z;
        z = copy.w * quaternion.z + copy.z * quaternion.w + copy.x * quaternion.y - copy.y * quaternion.x;
        w = copy.w * quaternion.w - copy.x * quaternion.x - copy.y * quaternion.y - copy.z * quaternion.z;

        return *this;
    }

    constexpr auto operator*(const Quaternion &quaternion) const -> Quaternion
    {
        return Quaternion{*this} *= quaternion;
    }

    float x;
    float y;
    float z;
    float w;
};
