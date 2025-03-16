#include "buffer.h"

#include <cstddef>
#include <cstdint>

#include "error.h"
#include "func.h"
#include "opengl.h"
#include "padding.h"

SECTION("buffer")
Buffer::Buffer(std::uint32_t size)
    : buffer_{}
    , size_{size}
{
    PADDING_LINE_THREE_QUARTER;
    ::glCreateBuffers(1, &buffer_);
    ::glNamedBufferStorage(buffer_, size, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
}

SECTION("buffer")
auto Buffer::write(const std::uint8_t *data, std::size_t size, std::size_t offset) const -> void
{
    PADDING_LINE_THREE_QUARTER;
    ensure(size_ >= size + offset, ErrorCode::BUFFER_TOO_SMALL);
    ::glNamedBufferSubData(buffer_, offset, size, data);
}

SECTION("buffer")
auto Buffer::native_handle() const -> ::GLuint
{
    PADDING_LINE_THREE_QUARTER;
    return buffer_;
}
