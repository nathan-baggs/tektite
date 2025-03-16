#include "vector3.h"

#include "clib.h"
#include "padding.h"

auto Vector3::distance(const Vector3 &v1, const Vector3 &v2) -> float
{
    PADDING_LINE_THREE_QUARTER;
    return hypot(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
