#include <Windows.h>

int main()
{
    const auto console = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if (console == INVALID_HANDLE_VALUE)
    {
        return 1;
    }

    const auto *msg = "hello world\n";

    ::DWORD written{};
    if (!::WriteConsoleA(console, msg, 12, &written, nullptr))
    {
        return 2;
    }

    return 0;
}
