#include "vector3.h"

#include "clib.h"
#include "padding.h"

namespace
{
}

auto Vector3::normalise(const Vector3 &v) -> Vector3
{
    PADDING_LINE_THREE_QUARTER;

    const auto length = hypot(v.x, v.y, v.z);
    if (length == 0.0f)
    {
        return {};
    }
    return {v.x / length, v.y / length, v.z / length};
}
