#pragma once

#include <array>

#include "clib.h"
#include "quaternion.h"
#include "vector3.h"

/**
 * Class representing a basic 4x4 matrix. The components are stored in column-major order so that they can be passed
 * directly to OpenGL.
 */
class Matrix4
{
  public:
    /**
     * Tag dispatch to disambiguate the constructor that takes a translation and a scale.
     */
    struct Scale
    {
    };

    /**
     * Default constructor, initialises the matrix to the identity matrix.
     */
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

    /**
     * Constructor that initialises the matrix with the specified elements.
     *
     * @param elements
     *   The elements to initialise the matrix with.
     */
    constexpr Matrix4(const std::array<float, 16u> &elements)
        : elements_(elements)
    {
    }

    /**
     * Constructor that initialises the matrix with the specified elements.
     *
     * Note that this is for keeping code simple.
     *
     * @param elements
     *   The elements to initialise the matrix with. Sure hope there's 16 of them!
     */
    Matrix4(const float *elements)
    {
        for (auto i = 0u; i < 16u; ++i)
        {
            elements_[i] = elements[i];
        }
    }

    /**
     * Construct a translation matrix.
     *
     * @param translation
     *   The translation to apply.
     */
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

    /**
     * Construct a scale matrix.
     *
     * @param scale
     *   The scale to apply.
     */
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

    /**
     * Construct a translation and scale matrix.
     *
     * @param translation
     *   The translation to apply.
     * @param scale
     *   The scale to apply.
     */
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

    /**
     * Construct a rotation matrix.
     *
     * @param rotation
     *   The rotation to apply.
     */
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

    /**
     * Create a look at (view) matrix.
     *
     * @param eye
     *   The position of the camera.
     * @param look_at
     *   The point to look at.
     * @param up
     *   The up vector.
     *
     * @return
     * The look at matrix.
     */
    static auto look_at(const Vector3 &eye, const Vector3 &look_at, const Vector3 &up) -> Matrix4;

    /**
     * Create a perspective (projection) matrix.
     *
     * @param fov
     *   The field of view in radians.
     * @param width
     *   The width of the viewport.
     * @param height
     *   The height of the viewport.
     * @param near_plane
     *   The near plane.
     * @param far_plane
     *   The far plane.
     *
     * @return
     * The perspective matrix.
     */
    static auto perspective(float fov, float width, float height, float near_plane, float far_plane) -> Matrix4;

    /**
     * Get the data of the matrix.
     *
     * Note that ideally we'd return a span or const ref to a std::array.
     *
     * @return
     * The data of the matrix.
     */
    constexpr auto data() const -> const float *
    {
        return elements_.data();
    }

    /**
     * Get an element of the matrix.
     *
     * @param self
     *   Deducing this pointer.
     * @param index
     *   The index of the element to get.
     *
     * @return
     *   The element at the specified index.
     */
    constexpr auto operator[](this auto &&self, std::size_t index) -> auto &
    {
        return self.elements_[index];
    }

    /**
     * Multiplication assignment operator.
     *
     * @param m1
     *   The matrix to multiply with.
     * @param m2
     *   The matrix to multiply.
     *
     * @return
     *   Reference to m1 after the multiplication.
     */
    friend constexpr auto operator*=(Matrix4 &m1, const Matrix4 &m2) -> Matrix4 &;

    /** Default equality operator. */
    constexpr auto operator==(const Matrix4 &) const -> bool = default;

  private:
    /** The elements of the matrix. */
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
