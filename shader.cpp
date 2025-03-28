#include "shader.h"

#include <utility>

#include "clib.h"
#include "error.h"
#include "log.h"
#include "opengl.h"

namespace
{

/**
 * Helper function to convert a game shader type to a native OpenGL shader type.
 *
 * @param type
 *   The type to convert.
 *
 * @return
 *   The native OpenGL shader type.
 */
auto to_native(ShaderType type) -> ::GLenum
{
    switch (type)
    {
        using enum ShaderType;
        case VERTEX: return GL_VERTEX_SHADER;
        case FRAGMENT: return GL_FRAGMENT_SHADER;
    }

    die(ErrorCode::UNKNOWN_SHADER_TYPE);
    std::unreachable();
}
}

Shader::Shader(const char *source, ShaderType type)
    : handle_{::glCreateShader(to_native(type))}
    , type_(type)
{
    const ::GLchar *strings[] = {source};
    const ::GLint lengths[] = {static_cast<::GLint>(strlen(source))};

    ::glShaderSource(handle_, 1, strings, lengths);
    ::glCompileShader(handle_);

    ::GLint result{};
    ::glGetShaderiv(handle_, GL_COMPILE_STATUS, &result);

    if (result != GL_TRUE)
    {
        char shader_log[512];
        ::glGetShaderInfoLog(handle_, sizeof(shader_log), nullptr, shader_log);

        log(shader_log);

        ensure(result, ErrorCode::FAILED_TO_COMPILE_SHADER);
    }
}

auto Shader::type() const -> ShaderType
{
    return type_;
}

auto Shader::native_handle() const -> ::GLuint
{
    return handle_;
}
