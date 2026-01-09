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


#ifndef PHYSICS_H
#define PHYSICS_H

#include "raylib.h"
#include <stdbool.h>

typedef struct GameObject GameObject;

typedef struct
{
    float walkSpeed;
    float runSpeed;
    float jumpForce;
    float gravity;
    float playerHeight;
    float playerRadius;
    float airControl;
    float groundFriction;
} PlayerPhysicsSettings;

typedef struct
{
    float mass;
    Vector3 velocity;
    Vector3 acceleration;
    bool isGrounded;
    float bounceFactor;
    float friction;
} PhysicsProperties;

void InitPhysics();
void SetGravity(float gravity);
void SetPlayerPhysicsSettings(float walkSpeed, float runSpeed, float jumpForce, 
                             float gravity, float playerHeight, float playerRadius,
                             float airControl, float groundFriction);

void UpdatePhysics(float deltaTime);
void UpdatePlayerPhysics(float deltaTime);
void ApplyForce(GameObject* obj, Vector3 force);

bool CheckCollision(GameObject* a, GameObject* b);
bool CheckAABBCollision(GameObject* a, GameObject* b);
bool CheckSphereAABBCollision(Vector3 sphereCenter, float sphereRadius, GameObject* box);
bool CheckSphereCollision(Vector3 c1, float r1, Vector3 c2, float r2);

void ApplyPhysicsToObject(GameObject* obj, float dt);
void ResolveCollision(GameObject* a, GameObject* b);
void ResolvePlayerCollision(GameObject* player, GameObject* other);

#endif