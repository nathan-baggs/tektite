#pragma once

#include <windows.h>

#include "event.h"

class Window
{
  public:
    Window(int width, int height);

    auto running() const -> bool;
    auto pump_message(Event *evt) -> bool;
    auto swap() const -> void;

  private:
    bool running_;
    ::WNDCLASS wc_;
    ::HWND window_;
    ::HDC dc_;
};
