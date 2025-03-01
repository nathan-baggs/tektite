#pragma once

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

    float x;
    float y;
    float z;
    float w;
};
