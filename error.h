#pragma once

#include <Windows.h>

/**
 * Enumeration of error codes.
 */
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
    HEAP_ALLOC_FAILED = 16,
    FAILED_TO_OPEN_WAVE_OUTPUT_DEVICE = 17,
    FAILED_TO_PREPARE_WAVE_HEADER = 18,
    FAILED_TO_WRITE_WAVE_OUTPUT = 19,
    FAILED_TO_UNPREPARE_WAVE_HEADER = 20,
};

/**
 * Exit the process with the given error code.
 *
 * @param ec
 *   The error code to exit with.
 */
inline auto die(ErrorCode ec) -> void
{
    ::ExitProcess(static_cast<::UINT>(ec));
}

/**
 * Ensure the condition is true, otherwise exit with the given error code.
 *
 * @param condition
 *   The condition to check.
 *
 * @param ec
 *   The error code to exit with.
 */
inline auto ensure(bool condition, ErrorCode ec) -> void
{
    if (!condition)
    {
        die(ec);
    }
}
