#pragma once

#include "opengl.h"

/**
 * Enumeration of shader types.
 */
enum class ShaderType
{
    VERTEX,
    FRAGMENT
};

/**
 * Class representing a shader.
 *
 * Note that for simplicity no cleanup is performed.
 */
class Shader
{
  public:
    /**
     * Construct a new shader.
     *
     * @param source
     *   The source code of the shader.
     * @param type
     *   The type of the shader.
     */
    Shader(const char *source, ShaderType type);

    /**
     * Get the shader type.
     *
     * @return
     *  The shader type.
     */
    auto type() const -> ShaderType;

    /**
     * Get the native handle of the shader.
     *
     * @return
     * The native handle of the shader.
     */
    auto native_handle() const -> ::GLuint;

  private:
    ::GLuint handle_;
    ShaderType type_;
};
