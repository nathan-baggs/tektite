#include "mesh.h"

#include "buffer.h"
#include "error.h"
#include "opengl.h"
#include "vertex_data.h"

Mesh::Mesh(
    const VertexData *vertex_data,
    std::uint32_t vertex_count,
    const std::uint32_t *indices,
    std::uint32_t index_count)
    : vao_{}
    , vbo_(sizeof(VertexData) * vertex_count + sizeof(std::uint32_t) * index_count)
    , index_count_{index_count}
    , index_offset_{sizeof(VertexData) * vertex_count}
{
    vbo_.write(reinterpret_cast<const std::uint8_t *>(vertex_data), sizeof(VertexData) * vertex_count, 0u);
    vbo_.write(
        reinterpret_cast<const std::uint8_t *>(indices),
        sizeof(std::uint32_t) * index_count,
        sizeof(VertexData) * vertex_count);

    ::glCreateVertexArrays(1, &vao_);
    ::glVertexArrayVertexBuffer(vao_, 0, vbo_.native_handle(), 0, sizeof(VertexData));
    ::glVertexArrayElementBuffer(vao_, vbo_.native_handle());

    ::glEnableVertexArrayAttrib(vao_, 0);
    ::glEnableVertexArrayAttrib(vao_, 1);

    ::glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, position));
    ::glVertexArrayAttribFormat(vao_, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, colour));

    ::glVertexArrayAttribBinding(vao_, 0, 0);
    ::glVertexArrayAttribBinding(vao_, 1, 0);
}

auto Mesh::bind() const -> void
{
    ::glBindVertexArray(vao_);
}

auto Mesh::unbind() const -> void
{
    ::glBindVertexArray(0);
}

auto Mesh::index_count() const -> std::uint32_t
{
    return index_count_;
}

auto Mesh::index_offset() const -> std::uintptr_t
{
    return index_offset_;
}
