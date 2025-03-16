#include "mesh.h"

#include "buffer.h"
#include "error.h"
#include "opengl.h"
#include "padding.h"
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
    PADDING_LINE_THREE_QUARTER;
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
    ::glEnableVertexArrayAttrib(vao_, 2);
    ::glEnableVertexArrayAttrib(vao_, 3);

    ::glVertexArrayAttribFormat(vao_, 0, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, position));
    ::glVertexArrayAttribFormat(vao_, 1, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, normal));
    ::glVertexArrayAttribFormat(vao_, 2, 3, GL_FLOAT, GL_FALSE, offsetof(VertexData, tangent));
    ::glVertexArrayAttribFormat(vao_, 3, 2, GL_FLOAT, GL_FALSE, offsetof(VertexData, uv));

    ::glVertexArrayAttribBinding(vao_, 0, 0);
    ::glVertexArrayAttribBinding(vao_, 1, 0);
    ::glVertexArrayAttribBinding(vao_, 2, 0);
    ::glVertexArrayAttribBinding(vao_, 3, 0);
}

auto Mesh::bind() const -> void
{
    PADDING_LINE_THREE_QUARTER;
    ::glBindVertexArray(vao_);
}

auto Mesh::unbind() const -> void
{
    PADDING_LINE_THREE_QUARTER;
    ::glBindVertexArray(0);
}

auto Mesh::index_count() const -> std::uint32_t
{
    PADDING_LINE_THREE_QUARTER;
    return index_count_;
}

auto Mesh::index_offset() const -> std::uintptr_t
{
    PADDING_LINE_THREE_QUARTER;
    return index_offset_;
}
