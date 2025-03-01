#pragma once

#include <cstddef>
#include <cstdint>

#include "opengl.h"

class Buffer
{
  public:
    Buffer(std::uint32_t size);

    auto write(const std::uint8_t *data, std::size_t size, std::size_t offset) const -> void;

    auto native_handle() const -> ::GLuint;

  private:
    ::GLuint buffer_;
    std::uint32_t size_;
};
