#pragma once

#include <cstdint>

#include "buffer.h"
#include "opengl.h"
#include "vertex_data.h"

class Mesh
{
  public:
    Mesh(
        const VertexData *vertex_data,
        std::uint32_t vertex_count,
        const std::uint32_t *indices,
        std::uint32_t index_count);

    auto bind() const -> void;
    auto unbind() const -> void;
    auto index_count() const -> std::uint32_t;
    auto index_offset() const -> std::uintptr_t;

  private:
    ::GLuint vao_;
    Buffer vbo_;
    std::uint32_t index_count_;
    std::uintptr_t index_offset_;
};
