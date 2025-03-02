#pragma once

#include <cstddef>

inline auto strlen(const char *str) -> std::size_t
{
    auto len = std::size_t{};

    while (str[len] != '\0')
    {
        ++len;
    }

    return len;
}

inline constexpr auto sin(float x) -> float
{
    return x - ((x * x * x) / 6.0f) + ((x * x * x * x * x) / 120.0f) - ((x * x * x * x * x * x * x) / 5040.0f) +
           ((x * x * x * x * x * x * x * x * x) / 362880.0f) -
           ((x * x * x * x * x * x * x * x * x * x * x) / 39916800.0f);
}

inline constexpr auto cos(float x) -> float
{
    return 1.0f - ((x * x) / 2.0f) + ((x * x * x * x) / 24.0f) - ((x * x * x * x * x * x) / 720.0f) +
           ((x * x * x * x * x * x * x * x) / 40320.0f) - ((x * x * x * x * x * x * x * x * x * x * x) / 3628800.0f);
}

inline constexpr auto tan(float x) -> float
{
    return sin(x) / cos(x);
}

inline constexpr auto sqrt(float x) -> float
{
    auto xn = x;

    for (auto i = 0u; i < 10u; ++i)
    {
        xn = 0.5f * (xn + (x / xn));
    }

    return xn;
}

inline constexpr auto hypot(float x, float y, float z) -> float
{
    return sqrt((x * x) + (y * y) + (z * z));
}
