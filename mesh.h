#pragma once

#include <cstdint>

#include "buffer.h"
#include "opengl.h"
#include "vertex_data.h"

/**
 * Class representing a mesh on the GPU.
 */
class Mesh
{
    /**
     * Construct a new mesh.
     *
     * @param vertex_data
     *   The vertex data.
     * @param vertex_count
     *   The number of vertices.
     * @param indices
     *   The indices.
     * @param index_count
     *  The number of indices.
     */
  public:
    Mesh(
        const VertexData *vertex_data,
        std::uint32_t vertex_count,
        const std::uint32_t *indices,
        std::uint32_t index_count);

    /**
     * Bind the mesh for rendering.
     */
    auto bind() const -> void;

    /**
     * Unbind the mesh.
     */
    auto unbind() const -> void;

    /**
     * Get the number of indices.
     *
     * @return
     *  The number of indices.
     */
    auto index_count() const -> std::uint32_t;

    /**
     * Get the offset of the indices into the GPU buffer.
     *
     * @return
     * The offset of the indices.
     */
    auto index_offset() const -> std::uintptr_t;

  private:
    /** The vertex array object. */
    ::GLuint vao_;

    /** The vertex buffer object. */
    Buffer vbo_;

    /** The number of indices. */
    std::uint32_t index_count_;

    /** The offset of the indices. */
    std::uintptr_t index_offset_;
};
