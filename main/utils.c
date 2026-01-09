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

#include "utils.h"
#include "engine.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>

static float deltaTime = 0.0f;
static double lastUpdateTime = 0.0;

float GetDeltaTime()
{
    return deltaTime;
}

void UpdateDeltaTime()  
{
    double currentTime = GetTime();
    deltaTime = (float)(currentTime - lastUpdateTime);
    lastUpdateTime = currentTime;
    
    if (deltaTime > 0.1f) deltaTime = 0.1f; 
}

Vector3 GetPlayerPosition()
{
    GameObject** playerObj = GetPlayerObject();
    if (*playerObj)
    {
        return (*playerObj)->position;
    }
    return (Vector3){0, 0, 0};
}

bool IsPlayerGrounded()
{
    GameObject** playerObj = GetPlayerObject();
    return *playerObj && (*playerObj)->physics.isGrounded;
}

float Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

Vector3 Vector3Lerp(Vector3 a, Vector3 b, float t)
{
    return (Vector3){
        Lerp(a.x, b.x, t),
        Lerp(a.y, b.y, t),
        Lerp(a.z, b.z, t)
    };
}

float Clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float RandomFloat(float min, float max)
{
    static int initialized = 0;
    if (!initialized)
    {
        srand((unsigned int)time(NULL));
        initialized = 1;
    }
    return min + (rand() / (float)RAND_MAX) * (max - min);
}

int RandomInt(int min, int max)
{
    static int initialized = 0;
    if (!initialized)
    {
        srand((unsigned int)time(NULL));
        initialized = 1;
    }
    return min + rand() % (max - min + 1);
}

Color RandomColor()
{
    return (Color){
        (unsigned char)RandomInt(0, 255),
        (unsigned char)RandomInt(0, 255),
        (unsigned char)RandomInt(0, 255),
        255
    };
}