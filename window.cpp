#include "window.h"

#include <GL/gl.h>
#include <hidusage.h>
#include <windows.h>

#include "third_party/opengl/wglext.h"

#include "error.h"
#include "event.h"
#include "log.h"
#include "opengl.h"

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB{};
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB{};

Event g_event;
bool g_has_event = false;

auto APIENTRY opengl_debug_callback(
    ::GLenum source,
    ::GLenum type,
    ::GLuint id,
    ::GLenum severity,
    ::GLsizei,
    const ::GLchar *message,
    const void *) -> void
{
    if (type == GL_DEBUG_TYPE_ERROR)
    {
        log(message);
    }
}

auto CALLBACK window_proc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) -> LRESULT
{
    switch (Msg)
    {
        case WM_CLOSE: ::PostQuitMessage(0); break;
        case WM_KEYUP:
        {
            g_event = Event{.type = EventType::KEY_UP, .data = static_cast<std::uint8_t>(wParam)};
            g_has_event = true;
            break;
        }
        case WM_KEYDOWN:
        {
            g_event = Event{.type = EventType::KEY_DOWN, .data = static_cast<std::uint8_t>(wParam)};
            g_has_event = true;
            break;
        }
        case WM_INPUT:
        {
            auto raw = ::RAWINPUT{};
            auto dwSize = ::UINT{sizeof(::RAWINPUT)};
            ::GetRawInputData(
                reinterpret_cast<::HRAWINPUT>(lParam), RID_INPUT, &raw, &dwSize, sizeof(::RAWINPUTHEADER));

            if (raw.header.dwType == RIM_TYPEMOUSE)
            {
                const auto x = raw.data.mouse.lLastX;
                const auto y = raw.data.mouse.lLastY;

                g_event = {
                    .type = EventType::MOUSE_MOVE,
                    .data = {.mouse_move = {static_cast<float>(x), static_cast<float>(y)}}};
                g_has_event = true;
            }

            break;
        }
        case WM_LBUTTONDOWN:
        {
            g_event = {.type = EventType::LEFT_MOUSE_CLICK};
            g_has_event = true;
            break;
        }
    }

    return ::DefWindowProc(hWnd, Msg, wParam, lParam);
}

auto resolve_gl_function(void **function, const char *name) -> void
{
    const auto address = ::wglGetProcAddress(name);
    ensure(address != nullptr, ErrorCode::RESOLVE_GL_FUNCTION);

    *function = reinterpret_cast<void *>(address);
}

auto resolve_wgl_functions(HINSTANCE instance) -> void
{
    auto wc = ::WNDCLASS{
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = ::DefWindowProc,
        .hInstance = instance,
        .lpszClassName = "dummy window"};

    ::RegisterClassA(&wc);

    auto dummy_window = ::CreateWindowExA(
        0,
        wc.lpszClassName,
        wc.lpszClassName,
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        wc.hInstance,
        0);

    auto dc = ::GetDC(dummy_window);

    auto pfd = ::PIXELFORMATDESCRIPTOR{
        .nSize = sizeof(::PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 32,
        .cAlphaBits = 8,
        .cDepthBits = 24,
        .cStencilBits = 8,
        .iLayerType = PFD_MAIN_PLANE};

    auto pixel_format = ::ChoosePixelFormat(dc, &pfd);

    ::SetPixelFormat(dc, pixel_format, &pfd);

    const auto context = ::wglCreateContext(dc);

    auto make_current_res = ::wglMakeCurrent(dc, context);
    ensure(make_current_res == TRUE, ErrorCode::WGL_MAKE_CURRENT);

    resolve_gl_function(reinterpret_cast<void **>(&wglCreateContextAttribsARB), "wglCreateContextAttribsARB");
    resolve_gl_function(reinterpret_cast<void **>(&wglChoosePixelFormatARB), "wglChoosePixelFormatARB");

    make_current_res = ::wglMakeCurrent(dc, 0);
    ensure(make_current_res == TRUE, ErrorCode::WGL_MAKE_CURRENT);

    ::ReleaseDC(dummy_window, dc);
    ::DestroyWindow(dummy_window);
}

auto init_opengl(HDC dc) -> void
{
    int pixel_format_attribs[]{
        WGL_DRAW_TO_WINDOW_ARB,
        GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,
        GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,
        GL_TRUE,
        WGL_ACCELERATION_ARB,
        WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,
        WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,
        32,
        WGL_DEPTH_BITS_ARB,
        24,
        WGL_STENCIL_BITS_ARB,
        8,
        0};

    auto pixel_format = 0;
    auto num_formats = UINT{};

    ::wglChoosePixelFormatARB(dc, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);

    auto pfd = ::PIXELFORMATDESCRIPTOR{};
    ::DescribePixelFormat(dc, pixel_format, sizeof(pfd), &pfd);
    ::SetPixelFormat(dc, pixel_format, &pfd);

    int gl_attribs[]{
        WGL_CONTEXT_MAJOR_VERSION_ARB,
        4,
        WGL_CONTEXT_MINOR_VERSION_ARB,
        6,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };

    auto context = ::wglCreateContextAttribsARB(dc, 0, gl_attribs);

    const auto make_current_res = ::wglMakeCurrent(dc, context);
    ensure(make_current_res == TRUE, ErrorCode::WGL_MAKE_CURRENT);
}

auto resolve_global_gl_functions() -> void
{
#define RESOLVE(TYPE, NAME) resolve_gl_function(reinterpret_cast<void **>(&NAME), #NAME);

    FOR_OPENGL_FUNCTIONS(RESOLVE)
}

auto setup_debug() -> void
{
    ::glEnable(GL_DEBUG_OUTPUT);
    ::glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    ::glDebugMessageCallback(opengl_debug_callback, nullptr);
}

Window::Window(int width, int height)
    : running_{true}
    , wc_{}
    , window_{}
    , dc_{}
{
    wc_ = ::WNDCLASS{
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .lpfnWndProc = window_proc,
        .hInstance = ::GetModuleHandleA(nullptr),
        .lpszClassName = "a"};

    const auto register_class_res = ::RegisterClassA(&wc_);
    ensure(register_class_res != 0, ErrorCode::REGISTER_CLASS);

    auto rect = ::RECT{.left = {}, .top = {}, .right = static_cast<int>(width), .bottom = static_cast<int>(height)};

    const auto adjust_window_res = ::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
    ensure(adjust_window_res != 0, ErrorCode::ADJUST_WINDOW_RECT);

    window_ = ::CreateWindowExA(
        0,
        wc_.lpszClassName,
        "",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        wc_.hInstance,
        nullptr);

    dc_ = ::GetDC(window_);

    ::ShowWindow(window_, SW_SHOW);
    ::UpdateWindow(window_);

    const auto rid = ::RAWINPUTDEVICE{
        .usUsagePage = HID_USAGE_PAGE_GENERIC,
        .usUsage = HID_USAGE_GENERIC_MOUSE,
        .dwFlags = RIDEV_INPUTSINK,
        .hwndTarget = window_};

    const auto register_raw_input_res = ::RegisterRawInputDevices(&rid, 1, sizeof(rid));
    ensure(register_raw_input_res == TRUE, ErrorCode::REGISTER_RAW_INPUT_DEVICES);

    resolve_wgl_functions(wc_.hInstance);
    init_opengl(dc_);
    resolve_global_gl_functions();
    setup_debug();

    ::glEnable(GL_DEPTH_TEST);

    g_has_event = false;
}

auto Window::running() const -> bool
{
    return running_;
}

auto Window::pump_message(Event *evt) -> bool
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

        if (g_has_event)
        {
            *evt = g_event;
            g_has_event = false;
            return true;
        }
    }

    return false;
}

auto Window::swap() const -> void
{
    ::SwapBuffers(dc_);
}
