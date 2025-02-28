#include "window.h"

#include <Windows.h>
#include <hidusage.h>

#include "error.h"

auto CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
    switch (Msg)
    {
        case WM_CLOSE: ::PostQuitMessage(0); break;
    }

    return ::DefWindowProc(hWnd, Msg, wParam, lParam);
}

Window::Window(int width, int height)
    : running_{true}
    , wc_{}
    , window_{}
    , dc_{}
{
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
}

auto Window::running() const -> bool
{
    return running_;
}

auto Window::pump_message() -> void
{
    auto msg = MSG{};
    while (::PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            running_ = false;
        }

        ::TranslateMessage(&msg);
        ::DispatchMessageA(&msg);
    }
}
