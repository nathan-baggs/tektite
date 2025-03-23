#pragma once

#include <cstddef>
#include <cstdint>

#include "opengl.h"

/**
 * Class representing an OpenGL buffer.
 *
 * Note that for simplicity we omit cleanup
 */
class Buffer
{
  public:
    /**
     * Construct a new OpenGL buffer with a given size.
     *
     * @param size
     *   Size on bytes of buffer.
     */
    Buffer(std::uint32_t size);

    /**
     * Write data to the OpenGL buffer.
     *
     * @param data
     *   Data to write.
     *
     * @param
     *   Size in bytes of data to write.
     *
     * @param offset
     *   Offset into the OpenGL buffer to start writing from.
     */
    auto write(const std::uint8_t *data, std::size_t size, std::size_t offset) const -> void;

    /**
     * Get the OpenGL buffer handle.
     *
     * @returns
     *   OpenGL buffer handle.
     */
    auto native_handle() const -> ::GLuint;

  private:
    /** OpenGL buffer handle. */
    ::GLuint buffer_;

    /** The size in bytes of the buffer. */
    std::uint32_t size_;
};
