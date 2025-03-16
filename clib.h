#pragma once

#include <cstddef>
#include <cstdint>

#include <windows.h>

#include <intrin.h>
#include <ntsecapi.h>

#include "error.h"

inline auto log(const char *msg) -> void;

inline auto my_strlen(const char *str) -> std::size_t
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
    const auto normalise_angle = [](float angle)
    {
        while (angle < -M_PI)
        {
            angle += 2.0f * M_PI;
        }

        while (angle > M_PI)
        {
            angle -= 2.0f * M_PI;
        }

        return angle;
    };

    x = normalise_angle(x);

    return x - ((x * x * x) / 6.0f) + ((x * x * x * x * x) / 120.0f) - ((x * x * x * x * x * x * x) / 5040.0f) +
           ((x * x * x * x * x * x * x * x * x) / 362880.0f);
}

inline constexpr auto cos(float x) -> float
{
    const auto normalise_angle = [](float angle)
    {
        while (angle < -M_PI)
        {
            angle += 2.0f * M_PI;
        }

        while (angle > M_PI)
        {
            angle -= 2.0f * M_PI;
        }

        return angle;
    };

    x = normalise_angle(x);

    return 1.0f - ((x * x) / 2.0f) + ((x * x * x * x) / 24.0f) - ((x * x * x * x * x * x) / 720.0f) +
           ((x * x * x * x * x * x * x * x) / 40320.0f);
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

inline auto malloc(std::size_t size) -> void *
{
    auto *ptr = ::HeapAlloc(::GetProcessHeap(), 0, size);
    ensure(ptr != nullptr, ErrorCode::HEAP_ALLOC_FAILED);

    return ptr;
}

inline auto free(void *ptr) -> void
{
    ::HeapFree(::GetProcessHeap(), 0, ptr);
}

inline auto my_memcpy(void *dest, const void *src, std::size_t size) -> void *
{
    ::__movsb(static_cast<std::uint8_t *>(dest), static_cast<const std::uint8_t *>(src), size);
    return dest;
}

inline auto memmove(void *dest, const void *src, std::size_t size) -> void *
{
    return ::MoveMemory(dest, src, size);
}

inline auto memcmp(const void *lhs, const void *rhs, std::size_t size) -> int
{
    const auto *lhs_bytes = static_cast<const std::uint8_t *>(lhs);
    const auto *rhs_bytes = static_cast<const std::uint8_t *>(rhs);

    for (auto i = 0u; i < size; ++i)
    {
        if (lhs_bytes[i] < rhs_bytes[i])
        {
            return -1;
        }
        else if (lhs_bytes[i] > rhs_bytes[i])
        {
            return 1;
        }
    }

    return 0;
}

inline auto rand() -> int
{
    char buffer[4]{};
    ::RtlGenRandom(buffer, sizeof(buffer));

    return *reinterpret_cast<int *>(buffer);
}
