#include <cstddef>

inline auto strlen(const char *str) -> std::size_t
{
    auto len = std::size_t{};

    while (str[len] != '\0')
    {
        ++len;
    }

    return len;
}
