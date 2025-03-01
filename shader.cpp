#include "shader.h"

#include <utility>

#include "clib.h"
#include "error.h"
#include "opengl.h"

namespace
{

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
        char log[512];
        ::glGetShaderInfoLog(handle_, sizeof(log), nullptr, log);

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
