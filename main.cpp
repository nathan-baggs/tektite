#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <hidusage.h>

enum class ErrorCode
{
    INVALID_STD_VALUE = 1,
    WRITE_CONSOLE = 2,
    REGISTER_CLASS = 3,
    ADJUST_WINDOW_RECT = 4,
    GET_DC = 5,
    SHOW_WINDOW = 6,
    UPDATE_WINDOW = 7,
    REGISTER_RAW_INPUT_DEVICES = 8
};

auto ensure(bool condition, ErrorCode ec) -> void
{
    if (!condition)
    {
        ::ExitProcess(static_cast<::UINT>(ec));
    }
}

auto strlen(const char *str) -> ::SIZE_T
{
    auto len = ::SIZE_T{};
    while (str[len] != '\0')
    {
        ++len;
    }

    return len;
}

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

auto CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
    switch (Msg)
    {
        case WM_CLOSE: ::PostQuitMessage(0); break;
    }

    return ::DefWindowProc(hWnd, Msg, wParam, lParam);
}

auto main() -> int
{
    static const auto width = 800u;
    static const auto height = 600u;

    log("hello, world");

    const auto wc = ::WNDCLASS{
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = window_proc,
        .hInstance = ::GetModuleHandleA(nullptr),
        .lpszClassName = "a"};

    const auto register_class_res = ::RegisterClassA(&wc);
    ensure(register_class_res != 0, ErrorCode::REGISTER_CLASS);

    auto rect = ::RECT{.left = {}, .top = {}, .right = static_cast<int>(width), .bottom = static_cast<int>(height)};

    const auto adjust_window_res = ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
    ensure(adjust_window_res != 0, ErrorCode::ADJUST_WINDOW_RECT);

    const auto window = ::CreateWindowExA(
        0,
        wc.lpszClassName,
        "",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr);

    const auto dc = ::GetDC(window);

    ::ShowWindow(window, SW_SHOW);
    ::UpdateWindow(window);

    const auto rid = ::RAWINPUTDEVICE{
        .usUsagePage = HID_USAGE_PAGE_GENERIC,
        .usUsage = HID_USAGE_GENERIC_MOUSE,
        .dwFlags = RIDEV_INPUTSINK,
        .hwndTarget = window};

    const auto register_raw_input_res = ::RegisterRawInputDevices(&rid, 1, sizeof(rid));
    ensure(register_raw_input_res == TRUE, ErrorCode::REGISTER_RAW_INPUT_DEVICES);

    log("starting");

    auto running = true;
    while (running)
    {
        auto msg = MSG{};
        while (::PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
            }

            ::TranslateMessage(&msg);
            ::DispatchMessageA(&msg);
        }
    }

    log("stopping\n");

    return 0;
}
