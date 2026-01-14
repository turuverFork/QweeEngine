//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Physics System Module
//==================================================================

#ifndef PHYSICS_H
#define PHYSICS_H

#include "raylib.h"
#include <stdbool.h>

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

typedef struct GameObject GameObject;

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