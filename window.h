#pragma once

#include <Windows.h>

class Window
{
  public:
    Window(int width, int height);

    auto running() const -> bool;
    auto pump_message() -> void;
    auto swap() const -> void;

  private:
    bool running_;
    ::WNDCLASS wc_;
    ::HWND window_;
    ::HDC dc_;
};
