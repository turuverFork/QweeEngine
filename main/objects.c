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

#include "objects.h"
#include "engine.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

GameObject* CreateObject(ObjectType type, const char* name, float x, float y, float z, 
                         bool physics, bool collision)
{
    GameObject** objects = GetObjects();
    int* objectCount = GetObjectCount();
    PlayerPhysicsSettings* settings = GetPlayerSettings();
    
    if (*objectCount >= MAX_OBJECTS)
    {
        printf("Warning: Maximum object limit reached!\n");
        return NULL;
    }
    
    GameObject* obj = (GameObject*)calloc(1, sizeof(GameObject));
    if (!obj) return NULL;
    
    obj->type = type;
    if (name)
    {
        snprintf(obj->name, sizeof(obj->name), "%s", name);
    }
    else
    {
        snprintf(obj->name, sizeof(obj->name), "Object_%d", *objectCount);
    }
    
    obj->position.x = x;
    obj->position.y = y;
    obj->position.z = z;
    
    obj->size.x = 1.0f;
    obj->size.y = 1.0f;
    obj->size.z = 1.0f;
    
    switch (type)
    {
        case OBJ_CUBE: obj->color = BLUE; break;
        case OBJ_SPHERE: 
            obj->color = RED;
            obj->size.x = 2.0f;
            obj->size.y = 2.0f;
            obj->size.z = 2.0f;
            break;
        case OBJ_PLAYER: 
            obj->color = GREEN;
            obj->size.x = settings->playerRadius * 2;
            obj->size.y = settings->playerHeight;
            obj->size.z = settings->playerRadius * 2;
            *GetPlayerObject() = obj;
            break;
        case OBJ_PYRAMID: obj->color = YELLOW; break;
        case OBJ_CYLINDER: obj->color = ORANGE; break;
        case OBJ_PLANE: 
            obj->color = GRAY;
            obj->isStatic = true;
            break;
        case OBJ_CONE: obj->color = MAGENTA; break;
        default: obj->color = WHITE; break;
    }
    
    obj->hasTexture = false;
    obj->isVisible = true;
    obj->hasPhysics = physics;
    obj->hasCollision = collision;
    obj->isActive = true;
    obj->isStatic = (type == OBJ_PLANE);
    
    if (physics)
    {
        obj->physics.mass = 1.0f;
        obj->physics.velocity = (Vector3){0, 0, 0};
        obj->physics.acceleration = (Vector3){0, 0, 0};
        obj->physics.isGrounded = false;
        obj->physics.bounceFactor = 0.5f;
        obj->physics.friction = 0.8f;
    }
    
    objects[(*objectCount)++] = obj;
    
    printf("Created object: %s at (%.1f, %.1f, %.1f)\n", obj->name, x, y, z);
    return obj;
}

void DestroyObject(GameObject* obj)
{
    GameObject** objects = GetObjects();
    int* objectCount = GetObjectCount();
    
    for (int i = 0; i < *objectCount; i++)
    {
        if (objects[i] == obj)
        {
            if (obj->hasTexture)
            {
                UnloadTexture(obj->texture);
            }
            
            if (obj == *GetPlayerObject())
            {
                *GetPlayerObject() = NULL;
            }
            
            free(obj);
            
            for (int j = i; j < *objectCount - 1; j++)
            {
                objects[j] = objects[j + 1];
            }
            (*objectCount)--;
            printf("Destroyed object: %s\n", obj->name);
            break;
        }
    }
}

GameObject* FindObject(const char* name)
{
    GameObject** objects = GetObjects();
    int* objectCount = GetObjectCount();
    
    for (int i = 0; i < *objectCount; i++)
    {
        if (objects[i] && strcmp(objects[i]->name, name) == 0)
        {
            return objects[i];
        }
    }
    return NULL;
}

void SetObjectPosition(GameObject* obj, float x, float y, float z)
{
    if (obj)
    {
        obj->position.x = x;
        obj->position.y = y;
        obj->position.z = z;
    }
}

void SetObjectScale(GameObject* obj, float sx, float sy, float sz)
{
    if (obj)
    {
        obj->size.x = sx;
        obj->size.y = sy;
        obj->size.z = sz;
    }
}

void SetObjectRotation(GameObject* obj, float rx, float ry, float rz)
{
    if (obj)
    {
        obj->rotation.x = rx;
        obj->rotation.y = ry;
        obj->rotation.z = rz;
    }
}

GameObject* CreateCube(const char* name, float x, float y, float z, 
                       bool physics, bool collision, 
                       const char* texturePath, Color color)
{
    GameObject* cube = CreateObject(OBJ_CUBE, name, x, y, z, physics, collision);
    if (cube)
    {
        cube->color = color;
        
        if (texturePath && texturePath[0] != '\0')
        {
            if (FileExists(texturePath))
            {
                cube->texture = LoadTexture(texturePath);
                cube->hasTexture = true;
            }
        }
    }
    return cube;
}

GameObject* CreateSphere(const char* name, float x, float y, float z, 
                         bool physics, bool collision, 
                         Color color, float radius)
{
    GameObject* sphere = CreateObject(OBJ_SPHERE, name, x, y, z, physics, collision);
    if (sphere)
    {
        sphere->color = color;
        sphere->size.x = radius * 2;
        sphere->size.y = radius * 2;
        sphere->size.z = radius * 2;
    }
    return sphere;
}

GameObject* CreatePlayer(const char* name, float x, float y, float z, 
                         bool physics, bool collision)
{
    GameObject* player = CreateObject(OBJ_PLAYER, name, x, y, z, physics, collision);
    if (player)
    {
        player->color = GREEN;
        PlayerPhysicsSettings* settings = GetPlayerSettings();
        player->size.x = settings->playerRadius * 2;
        player->size.y = settings->playerHeight;
        player->size.z = settings->playerRadius * 2;
        player->physics.friction = settings->groundFriction;
    }
    return player;
}

GameObject* CreatePyramid(const char* name, float x, float y, float z, 
                          bool physics, bool collision, 
                          const char* texturePath, Color color)
{
    GameObject* pyramid = CreateObject(OBJ_PYRAMID, name, x, y, z, physics, collision);
    if (pyramid)
    {
        pyramid->color = color;
        
        if (texturePath && texturePath[0] != '\0')
        {
            if (FileExists(texturePath))
            {
                pyramid->texture = LoadTexture(texturePath);
                pyramid->hasTexture = true;
            }
        }
    }
    return pyramid;
}

GameObject* CreateCylinder(const char* name, float x, float y, float z, 
                           bool physics, bool collision, 
                           Color color, float radius, float height)
{
    GameObject* cylinder = CreateObject(OBJ_CYLINDER, name, x, y, z, physics, collision);
    if (cylinder)
    {
        cylinder->color = color;
        cylinder->size.x = radius * 2;
        cylinder->size.y = height;
        cylinder->size.z = radius * 2;
    }
    return cylinder;
}

GameObject* CreatePlane(const char* name, float x, float y, float z, 
                        float width, float depth, Color color)
{
    GameObject* plane = CreateObject(OBJ_PLANE, name, x, y, z, false, true);
    if (plane)
    {
        plane->color = color;
        plane->size.x = width;
        plane->size.z = depth;
        plane->size.y = 0.1f;
        plane->isStatic = true;
    }
    return plane;
}

GameObject* CreateCone(const char* name, float x, float y, float z, 
                       bool physics, bool collision, 
                       Color color, float radius, float height)
{
    GameObject* cone = CreateObject(OBJ_CONE, name, x, y, z, physics, collision);
    if (cone)
    {
        cone->color = color;
        cone->size.x = radius * 2;
        cone->size.y = height;
        cone->size.z = radius * 2;
    }
    return cone;
}

void DrawObject(GameObject* obj)
{
    if (!obj || !obj->isVisible) return;
    
    if (obj->type == OBJ_PLAYER) return;
    
    bool* wireframeMode = GetWireframeMode();
    
    if (*wireframeMode)
    {
        switch (obj->type)
        {
            case OBJ_CUBE:
            case OBJ_PYRAMID:
            case OBJ_PLANE:
                DrawCubeWires(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
                break;
            case OBJ_SPHERE:
                DrawSphereWires(obj->position, obj->size.x / 2, 16, 16, obj->color);
                break;
            case OBJ_CYLINDER:
            case OBJ_CONE:
                DrawCylinderWires(obj->position, obj->size.x / 2, obj->size.x / 2, obj->size.y, 16, obj->color);
                break;
            default:
                DrawCubeWires(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
                break;
        }
    }
    else
    {
        switch (obj->type)
        {
            case OBJ_CUBE:
                if (obj->hasTexture)
                {
                    DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
                }
                else
                {
                    DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
                }
                break;
            case OBJ_SPHERE:
                DrawSphere(obj->position, obj->size.x / 2, obj->color);
                break;
            case OBJ_PYRAMID:
                DrawCube(obj->position, obj->size.x, obj->size.y * 0.7f, obj->size.z, obj->color);
                break;
            case OBJ_CYLINDER:
                DrawCylinder(obj->position, obj->size.x / 2, obj->size.x / 2, obj->size.y, 16, obj->color);
                break;
            case OBJ_PLANE:
                DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
                break;
            case OBJ_CONE:
                DrawCylinder(obj->position, obj->size.x / 2, 0, obj->size.y, 16, obj->color);
                break;
            default:
                DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
                break;
        }
        
        if (obj->hasCollision)
        {
            DrawCubeWires(obj->position, obj->size.x, obj->size.y, obj->size.z, BLACK);
        }
    }
}