#pragma once

#include "clib.h"

/**
 * Class representing a basic 3d vector, with x, y and z components.
 */
struct Vector3
{
    /**
     * Default constructor, initialises all components to 0.
     */
    constexpr Vector3()
        : Vector3(0.0f)
    {
    }

    /**
     * Constructor that initialises all components to the same value.
     *
     * @param xyz
     *   The value to initialise all components to.
     */
    constexpr Vector3(float xyz)
        : Vector3(xyz, xyz, xyz)
    {
    }

    /**
     * Constructor that initialises all components to the specified values.
     *
     * @param x
     *   The value to initialise the x component to.
     * @param y
     *   The value to initialise the y component to.
     * @param z
     *   The value to initialise the z component to.
     */
    constexpr Vector3(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    /**
     * Normalises the vector.
     *
     * @param v
     *   The vector to normalise.
     *
     * @return
     *   The normalised vector.
     */
    static auto normalise(const Vector3 &v) -> Vector3
    {
        const auto length = hypot(v.x, v.y, v.z);
        if (length == 0.0f)
        {
            return {};
        }
        return {v.x / length, v.y / length, v.z / length};
    }

    /**
     * Calculates the cross product of two vectors.
     *
     * @param v1
     *   The first vector.
     * @param v2
     *   The second vector.
     * @return
     *  The cross product of the two vectors.
     */
    static auto cross(const Vector3 &v1, const Vector3 &v2) -> Vector3
    {
        const auto i = (v1.y * v2.z) - (v1.z * v2.y);
        const auto j = (v1.x * v2.z) - (v1.z * v2.x);
        const auto k = (v1.x * v2.y) - (v1.y * v2.x);

        return {i, -j, k};
    }

    /**
     * Calculates the dot product of two vectors.
     *
     * @param v1
     *   The first vector.
     * @param v2
     *   The second vector.
     *
     * @return
     * The dot product of the two vectors.
     */
    static auto distance(const Vector3 &v1, const Vector3 &v2) -> float
    {
        return hypot(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
    }

    /** Default equality operator. */
    auto operator==(const Vector3 &) const -> bool = default;

    /** The x component of the vector. */
    float x;

    /** The y component of the vector. */
    float y;

    /** The z component of the vector. */
    float z;
};

/**
 * Subtract assignment operator.
 *
 * @param v1
 *   The vector to subtract from.
 * @param v2
 *   The vector to subtract.
 *
 * @return
 *   Reference to v1 after the subtraction.
 */
inline auto operator-=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;

    return v1;
}

/**
 * Subtract operator.
 *
 * @param v1
 *   The vector to subtract from.
 * @param v2
 *  The vector to subtract.
 *
 * @return
 *  The vector after the subtraction.
 */
inline auto operator-(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    auto tmp = v1;
    return tmp -= v2;
}

/**
 * Addition assignment operator.
 *
 * @param v1
 *   The vector to add to.
 * @param v2
 *   The vector to add.
 *
 * @return
 *   Reference to v1 after the addition.
 */
inline auto operator+=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;

    return v1;
}

/**
 * Addition operator.
 *
 * @param v1
 *   The vector to add to.
 * @param v2
 *  The vector to add.
 *
 * @return
 *  The vector after the addition.
 */
inline auto operator+(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    auto tmp = v1;
    return tmp += v2;
}

/**
 * Multiplication (component-wise) assignment operator.
 *
 * @param v1
 *   The vector to multiple with.
 * @param v2
 *   The vector to multiple.
 *
 * @return
 *   Reference to v1 after the multiplication.
 */
inline auto operator*=(Vector3 &v1, const Vector3 &v2) -> Vector3 &
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;

    return v1;
}

/**
 * Multiplication (component-wise) operator.
 *
 * @param v1
 *   The vector to multiply with.
 * @param v2
 *  The vector to add.
 *
 * @return
 *  The vector after the multiplication.
 */
inline auto operator*(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    auto tmp = v1;
    return tmp *= v2;
}

/**
 * Negate operator.
 *
 * @param v
 *   The vector to negate.
 *
 * @return
 *  The negated vector.
 */
inline auto operator-(const Vector3 &v) -> Vector3
{
    return {-v.x, -v.y, -v.z};
}
