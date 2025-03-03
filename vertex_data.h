#pragma once

#include "vector3.h"

struct UV
{
    float x;
    float y;
};

struct VertexData
{
    Vector3 position;
    Vector3 normal;
    Vector3 tangent;
    UV uv;
};
