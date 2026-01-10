//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Vector Math Implementation
//==================================================================

#include "vector_math.h"
#include <math.h>

Vector3 Vector3Add(Vector3 v1, Vector3 v2)
{
    return (Vector3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

Vector3 Vector3Subtract(Vector3 v1, Vector3 v2)
{
    return (Vector3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

Vector3 Vector3Scale(Vector3 v, float scalar)
{
    return (Vector3){v.x * scalar, v.y * scalar, v.z * scalar};
}

float Vector3Length(Vector3 v)
{
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

Vector3 Vector3Normalize(Vector3 v)
{
    float length = Vector3Length(v);
    if (length == 0) return (Vector3){0, 0, 0};
    return (Vector3){v.x / length, v.y / length, v.z / length};
}

Vector3 Vector3CrossProduct(Vector3 v1, Vector3 v2)
{
    return (Vector3){
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

float Vector3DotProduct(Vector3 v1, Vector3 v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}