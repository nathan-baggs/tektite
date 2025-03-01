#pragma once

#include <cstdint>

#include "matrix4.h"
#include "opengl.h"
#include "shader.h"

class Material
{
  public:
    Material(const Shader &vertex_shader, const Shader &fragment_shader);

    auto use() const -> void;
    auto set_uniform(const char *name, const Matrix4 &obj) const -> void;
    auto set_uniform(const char *name, int obj) const -> void;
    auto set_uniform(const char *name, float obj) const -> void;

    auto native_handle() const -> ::GLuint;

  private:
    ::GLuint handle_;
};
