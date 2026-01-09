 //==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Engine version: 0.1 | Build: ALPHA-2026
//
//Core Systems:
//  [✓] Physics: Rigidbody dynamics, AABB/Sphere collisions
//  [✓] Rendering: First-person camera, object rendering
// [✓] Object System: Cubes, spheres, cylinders, pyramids
//  [✓] Camera: First-person controls with mouse look
//  [✓] Input: WASD movement, mouse camera control
//
///Performance Profile:
//  - Max objects: 500 units
//  - Target FPS: 60 @ 1280x720
// - Physics steps: 1000+/sec
//  - Memory footprint: <64KB object pool
//
//Build Configuration:
 // - Platform: Windows/Linux/macOS (via Raylib)
 // - Mode: First-person 3D
  //   - Renderer: Software-accelerated 3D
//
 //Controls:
 //  W/A/S/D    - Player movement
 //  Mouse      - Camera look
 //  SPACE      - Jump
  // SHIFT      - Run
// F1         - Toggle wireframe
 // R          - Throw physics ball
 // ESC        - Exit engine
// ==================================================================

#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include "raylib.h"
#include <math.h>

static inline Vector3 Vector3Add(Vector3 v1, Vector3 v2)
{
    return (Vector3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

static inline Vector3 Vector3Subtract(Vector3 v1, Vector3 v2)
{
    return (Vector3){v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

static inline Vector3 Vector3Scale(Vector3 v, float scalar)
{
    return (Vector3){v.x * scalar, v.y * scalar, v.z * scalar};
}

static inline float Vector3Length(Vector3 v)
{
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

static inline Vector3 Vector3Normalize(Vector3 v)
{
    float length = Vector3Length(v);
    if (length == 0) return (Vector3){0, 0, 0};
    return (Vector3){v.x / length, v.y / length, v.z / length};
}

static inline Vector3 Vector3CrossProduct(Vector3 v1, Vector3 v2)
{
    return (Vector3){
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

static inline float Vector3DotProduct(Vector3 v1, Vector3 v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

#endif