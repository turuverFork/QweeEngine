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
#ifndef ENGINE_H
#define ENGINE_H

#include "raylib.h"
#include "physics.h"
#include "objects.h"
#include "camera.h"
#include "utils.h"
#include "vector_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

extern GameObject* objects[MAX_OBJECTS];
extern int objectCount;
extern GameObject* playerObject;
extern Camera3D camera;
extern PlayerPhysicsSettings playerSettings;
extern bool wireframeMode;

void InitEngine(int screenWidth, int screenHeight, const char* title, bool fullscreen);
void CloseEngine();
void UpdateEngine(float deltaTime);
void RenderAll();
void ToggleWireframe(bool enabled);

GameObject** GetObjects();
int* GetObjectCount();
GameObject** GetPlayerObject();
Camera3D* GetCamera();
PlayerPhysicsSettings* GetPlayerSettings();
bool* GetWireframeMode();

#endif