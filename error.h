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
};

inline auto ensure(bool condition, ErrorCode ec) -> void
{
    if (!condition)
    {
        ::ExitProcess(static_cast<::UINT>(ec));
    }
}
