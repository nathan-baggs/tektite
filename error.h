#pragma once

#include <Windows.h>

enum class ErrorCode
{
    INVALID_STD_VALUE = 1,
    WRITE_CONSOLE = 2,
    REGISTER_CLASS = 3,
    ADJUST_WINDOW_RECT = 4,
    GET_DC = 5,
    SHOW_WINDOW = 6,
    UPDATE_WINDOW = 7,
    REGISTER_RAW_INPUT_DEVICES = 8,
    RESOLVE_GL_FUNCTION = 9,
    WGL_MAKE_CURRENT = 10,
    BUFFER_TOO_SMALL = 11,
    UNKNOWN_SHADER_TYPE = 12,
    FAILED_TO_COMPILE_SHADER = 13,
    FAILED_TO_CREATE_PROGRAM = 14,
    FAILED_TO_LINK_PROGRAM = 15,
};

inline auto die(ErrorCode ec) -> void
{
    ::ExitProcess(static_cast<::UINT>(ec));
}

inline auto ensure(bool condition, ErrorCode ec) -> void
{
    if (!condition)
    {
        die(ec);
    }
}
