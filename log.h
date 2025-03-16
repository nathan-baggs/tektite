#pragma once

#include <Windows.h>

#include "clib.h"
#include "error.h"

inline auto log(const char *msg) -> void
{
    const auto console = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ensure(console != INVALID_HANDLE_VALUE, ErrorCode::INVALID_STD_VALUE);

    ::DWORD written{};
    auto write_res = ::WriteConsoleA(console, msg, strlen(msg), &written, nullptr);

    written = {};
    const auto newline = "\n";
    write_res = ::WriteConsoleA(console, newline, 1, &written, nullptr);
}
