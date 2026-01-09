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

#ifndef OBJECTS_H
#define OBJECTS_H

#include "raylib.h"
#include "physics.h"
#include <stdbool.h>

typedef struct GameObject GameObject;

typedef enum
{
    OBJ_CUBE,
    OBJ_SPHERE,
    OBJ_PLAYER,
    OBJ_PYRAMID,
    OBJ_CYLINDER,
    OBJ_PLANE,
    OBJ_CONE,
    OBJ_CUSTOM
} ObjectType;

struct GameObject
{
    ObjectType type;
    char name[32];
    
    Vector3 position;
    Vector3 size;
    Vector3 rotation;
    Color color;
    
    Texture2D texture;
    bool hasTexture;
    bool isVisible;
    
    bool hasPhysics;
    PhysicsProperties physics;
    
    bool hasCollision;
    bool isTrigger;
    
    bool isActive;
    bool isStatic;
    
    void* customData;
};

#define MAX_OBJECTS 500

GameObject* CreateObject(ObjectType type, const char* name, float x, float y, float z, 
                         bool physics, bool collision);
void DestroyObject(GameObject* obj);
GameObject* FindObject(const char* name);
void SetObjectPosition(GameObject* obj, float x, float y, float z);
void SetObjectScale(GameObject* obj, float sx, float sy, float sz);
void SetObjectRotation(GameObject* obj, float rx, float ry, float rz);

GameObject* CreateCube(const char* name, float x, float y, float z, 
                       bool physics, bool collision, 
                       const char* texturePath, Color color);
GameObject* CreateSphere(const char* name, float x, float y, float z, 
                         bool physics, bool collision, 
                         Color color, float radius);
GameObject* CreatePlayer(const char* name, float x, float y, float z, 
                         bool physics, bool collision);
GameObject* CreatePyramid(const char* name, float x, float y, float z, 
                          bool physics, bool collision, 
                          const char* texturePath, Color color);
GameObject* CreateCylinder(const char* name, float x, float y, float z, 
                           bool physics, bool collision, 
                           Color color, float radius, float height);
GameObject* CreatePlane(const char* name, float x, float y, float z, 
                        float width, float depth, Color color);
GameObject* CreateCone(const char* name, float x, float y, float z, 
                       bool physics, bool collision, 
                       Color color, float radius, float height);

void DrawObject(GameObject* obj);

#endif