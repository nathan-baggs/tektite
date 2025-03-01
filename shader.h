#pragma once

#include "opengl.h"

enum class ShaderType
{
    VERTEX,
    FRAGMENT
};

class Shader
{
  public:
    Shader(const char *source, ShaderType type);

    auto type() const -> ShaderType;
    auto native_handle() const -> ::GLuint;

  private:
    ::GLuint handle_;
    ShaderType type_;
};
