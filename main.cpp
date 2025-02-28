#include <Windows.h>

#include "clib.h"
#include "error.h"
#include "window.h"

auto log(const char *msg) -> void
{
    const auto console = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ensure(console != INVALID_HANDLE_VALUE, ErrorCode::INVALID_STD_VALUE);

    ::DWORD written{};
    auto write_res = ::WriteConsoleA(console, msg, strlen(msg), &written, nullptr);
    ensure(write_res, ErrorCode::WRITE_CONSOLE);

    written = {};
    const auto newline = "\n";
    write_res = ::WriteConsoleA(console, newline, 1, &written, nullptr);
    ensure(write_res, ErrorCode::WRITE_CONSOLE);
}

auto main() -> int
{
    static const auto width = 1920u;
    static const auto height = 1080u;

    auto window = Window{width, height};

    log("starting");

    while (window.running())
    {
        window.pump_message();
    }

    log("stopping\n");

    return 0;
}
