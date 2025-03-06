#include "buffer.h"

#include <cstddef>
#include <cstdint>

#include "error.h"
#include "opengl.h"

Buffer::Buffer(std::uint32_t size)
    : buffer_{}
    , size_{size}
{
    ::glCreateBuffers(1, &buffer_);
    ::glNamedBufferStorage(buffer_, size, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
}

auto Buffer::write(const std::uint8_t *data, std::size_t size, std::size_t offset) const -> void
{
    ensure(size_ >= size + offset, ErrorCode::BUFFER_TOO_SMALL);
    ::glNamedBufferSubData(buffer_, offset, size, data);
}

auto Buffer::native_handle() const -> ::GLuint
{
    return buffer_;
}
