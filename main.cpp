#include <Windows.h>

#include "window.h"

#include "log.h"
#include "opengl.h"

// https://stackoverflow.com/a/1583220
EXTERN_C int _fltused = 0;

auto main() -> int
{
    static const auto width = 1920u;
    static const auto height = 1080u;

    log("starting");

    auto window = Window{width, height};

    while (window.running())
    {
        window.pump_message();

        ::glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.swap();
    }

    log("stopping");

    // avoid cleanup, just die
    ::ExitProcess(0);
}
