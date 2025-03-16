#include "vector3.h"

#include "padding.h"

__pragma(code_seg("v2")) auto Vector3::cross(const Vector3 &v1, const Vector3 &v2) -> Vector3
{
    PADDING_LINE_THREE_QUARTER;
    const auto i = (v1.y * v2.z) - (v1.z * v2.y);
    const auto j = (v1.x * v2.z) - (v1.z * v2.x);
    const auto k = (v1.x * v2.y) - (v1.y * v2.x);

    return {i, -j, k};
}
