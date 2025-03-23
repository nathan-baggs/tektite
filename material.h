#pragma once

#include <cstdint>

#include "matrix4.h"
#include "opengl.h"
#include "shader.h"

/**
 * Class representing a material i.e. a linkedn OpenGl program.
 *
 * Note that for simplicity no cleanup is performed.
 */
class Material
{
  public:
    /**
     * Construct a new material.
     *
     * @param vertex_shader
     *   The vertex shader.
     * @param fragment_shader
     *   The fragment shader.
     */
    Material(const Shader &vertex_shader, const Shader &fragment_shader);

    /**
     * Use the material.
     */
    auto use() const -> void;

    /**
     * Set a uniform matrix.
     *
     * @param name
     *   The name of the uniform.
     * @param obj
     *   The matrix to set.
     */
    auto set_uniform(const char *name, const Matrix4 &obj) const -> void;

    /**
     * Set a uniform integer.
     *
     * @param name
     *   The name of the uniform.
     * @param obj
     *   The integer to set.
     */
    auto set_uniform(const char *name, int obj) const -> void;

    /**
     * Set a uniform float.
     *
     * @param name
     *   The name of the uniform.
     * @param obj
     *   The float to set.
     */
    auto set_uniform(const char *name, float obj) const -> void;

    /**
     * Get the native handle of the material.
     *
     * @return
     *  The native handle of the material.
     */
    auto native_handle() const -> ::GLuint;

  private:
    /** The native handle of the material. */
    ::GLuint handle_;
};
