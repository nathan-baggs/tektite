#include "material.h"

#include "error.h"
#include "log.h"
#include "opengl.h"
#include "shader.h"

Material::Material(const Shader &vertex_shader, const Shader &fragment_shader)
    : handle_{::glCreateProgram()}
{
    ensure(handle_ != 0u, ErrorCode::FAILED_TO_CREATE_PROGRAM);

    ::glAttachShader(handle_, vertex_shader.native_handle());
    ::glAttachShader(handle_, fragment_shader.native_handle());
    ::glLinkProgram(handle_);

    ::GLint result{};
    ::glGetProgramiv(handle_, GL_LINK_STATUS, &result);

    if (result != GL_TRUE)
    {
        char error_log[512];
        ::glGetProgramInfoLog(handle_, sizeof(error_log), nullptr, error_log);

        log(error_log);
        die(ErrorCode::FAILED_TO_LINK_PROGRAM);
    }
}

auto Material::use() const -> void
{
    ::glUseProgram(handle_);
}

auto Material::set_uniform(const char *name, const Matrix4 &obj) const -> void
{
    if (const auto location = ::glGetUniformLocation(handle_, name); location != -1)
    {
        ::glUniformMatrix4fv(location, 1, GL_FALSE, obj.data());
    }
}

auto Material::set_uniform(const char *name, int obj) const -> void
{
    if (const auto location = ::glGetUniformLocation(handle_, name); location != -1)
    {
        ::glUniform1i(location, obj);
    }
}

auto Material::set_uniform(const char *name, float obj) const -> void
{
    if (const auto location = ::glGetUniformLocation(handle_, name); location != -1)
    {
        ::glUniform1f(location, obj);
    }
}

auto Material::native_handle() const -> ::GLuint
{
    return handle_;
}
