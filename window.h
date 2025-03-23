#pragma once

#include <Windows.h>

#include "event.h"

/**
 * Class representing an OpenGL window.
 *
 * Note that for simplicity we assume one window will be created and live for the lifetime of the game, so we omit all
 * clean-up code.
 */
class Window
{
  public:
    /**
     * Constructor that creates a window with the specified dimensions.
     *
     * @param width
     *   The width of the window.
     * @param height
     *   The height of the window.
     */
    Window(int width, int height);

    /**
     * Check if the window is running.
     *
     * @return
     *  True if the window is running, false otherwise.
     */
    auto running() const -> bool;

    /**
     * Get the next event from the window.
     *
     * Note that without optional a user should just loop until this returns false.
     *
     * @param evt
     *   The event to populate.
     *
     * @return
     *   True if an event was retrieved, false otherwise.
     */
    auto pump_message(Event *evt) -> bool;

    /**
     * Swap the buffers.
     */
    auto swap() const -> void;

  private:
    /** Flag indicating if the window is running. */
    bool running_;

    /** The window class. */
    ::WNDCLASS wc_;

    /** The window handle. */
    ::HWND window_;

    /** The device context. */
    ::HDC dc_;
};
