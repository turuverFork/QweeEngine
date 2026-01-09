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
#include "physics.h"
#include "engine.h"
#include <math.h>
#include <stdio.h>

static bool CheckPlayerCollision(GameObject* player, GameObject* other);

void InitPhysics()
{
}

void SetGravity(float gravity)
{
    PlayerPhysicsSettings* settings = GetPlayerSettings();
    settings->gravity = gravity;
}

void SetPlayerPhysicsSettings(float walkSpeed, float runSpeed, float jumpForce, 
                             float gravity, float playerHeight, float playerRadius,
                             float airControl, float groundFriction)
{
    PlayerPhysicsSettings* settings = GetPlayerSettings();
    settings->walkSpeed = walkSpeed;
    settings->runSpeed = runSpeed;
    settings->jumpForce = jumpForce;
    settings->gravity = gravity;
    settings->playerHeight = playerHeight;
    settings->playerRadius = playerRadius;
    settings->airControl = airControl;
    settings->groundFriction = groundFriction;
    
    GameObject** playerObj = GetPlayerObject();
    if (*playerObj)
    {
        (*playerObj)->size.x = playerRadius * 2;
        (*playerObj)->size.y = playerHeight;
        (*playerObj)->size.z = playerRadius * 2;
    }
    
    printf("Player physics settings updated:\n");
    printf("  Walk Speed: %.1f\n", walkSpeed);
    printf("  Run Speed: %.1f\n", runSpeed);
    printf("  Jump Force: %.1f\n", jumpForce);
    printf("  Gravity: %.1f\n", gravity);
}

void ApplyPhysicsToObject(GameObject* obj, float dt)
{
    if (!obj->hasPhysics || obj->isStatic) return;
    
    PlayerPhysicsSettings* settings = GetPlayerSettings();
    
    if (obj->type != OBJ_PLAYER)
    {
        obj->physics.velocity.y += settings->gravity * dt;
    }
    
    obj->position.x += obj->physics.velocity.x * dt;
    obj->position.y += obj->physics.velocity.y * dt;
    obj->position.z += obj->physics.velocity.z * dt;
    
    if (obj->type != OBJ_PLAYER)
    {
        float groundLevel = obj->size.y / 2;
        
        if (obj->position.y <= groundLevel)
        {
            obj->position.y = groundLevel;
            obj->physics.velocity.y = -obj->physics.velocity.y * obj->physics.bounceFactor;
            obj->physics.isGrounded = true;
            
            if (fabs(obj->physics.velocity.y) < 0.1f)
            {
                obj->physics.velocity.y = 0;
            }
            
            obj->physics.velocity.x *= obj->physics.friction;
            obj->physics.velocity.z *= obj->physics.friction;
        }
        else
        {
            obj->physics.isGrounded = false;
        }
    }
    
    if (!obj->physics.isGrounded)
    {
        obj->physics.velocity.x *= 0.99f;
        obj->physics.velocity.z *= 0.99f;
    }
}

void UpdatePhysics(float deltaTime)
{
    GameObject** objects = GetObjects();
    int* objectCount = GetObjectCount();
    
    for (int i = 0; i < *objectCount; i++)
    {
        if (objects[i] && objects[i]->hasPhysics && objects[i]->isActive && 
            objects[i]->type != OBJ_PLAYER)
        {
            ApplyPhysicsToObject(objects[i], deltaTime);
        }
    }
    
    for (int i = 0; i < *objectCount; i++)
    {
        if (!objects[i] || !objects[i]->hasCollision || !objects[i]->isActive) continue;
        
        for (int j = i + 1; j < *objectCount; j++)
        {
            if (!objects[j] || !objects[j]->hasCollision || !objects[j]->isActive) continue;
            
            if (CheckAABBCollision(objects[i], objects[j]) || 
                (objects[i]->type == OBJ_PLAYER && CheckPlayerCollision(objects[i], objects[j])) ||
                (objects[j]->type == OBJ_PLAYER && CheckPlayerCollision(objects[j], objects[i])))
            {
                ResolveCollision(objects[i], objects[j]);
            }
        }
    }
}

void UpdatePlayerPhysics(float deltaTime)
{
    GameObject** playerObj = GetPlayerObject();
    PlayerPhysicsSettings* settings = GetPlayerSettings();
    
    if (!*playerObj) return;
    
    if (!(*playerObj)->physics.isGrounded)
    {
        (*playerObj)->physics.velocity.y += settings->gravity * deltaTime;
    }
    
    (*playerObj)->position.x += (*playerObj)->physics.velocity.x * deltaTime;
    (*playerObj)->position.y += (*playerObj)->physics.velocity.y * deltaTime;
    (*playerObj)->position.z += (*playerObj)->physics.velocity.z * deltaTime;
    
    float groundLevel = settings->playerHeight / 2;
    if ((*playerObj)->position.y <= groundLevel)
    {
        (*playerObj)->position.y = groundLevel;
        (*playerObj)->physics.velocity.y = 0;
        (*playerObj)->physics.isGrounded = true;
    }
    
    if ((*playerObj)->physics.isGrounded)
    {
        (*playerObj)->physics.velocity.x *= settings->groundFriction;
        (*playerObj)->physics.velocity.z *= settings->groundFriction;
    }
}

void ApplyForce(GameObject* obj, Vector3 force)
{
    if (obj && obj->hasPhysics)
    {
        obj->physics.velocity.x += force.x / obj->physics.mass;
        obj->physics.velocity.y += force.y / obj->physics.mass;
        obj->physics.velocity.z += force.z / obj->physics.mass;
    }
}

bool CheckAABBCollision(GameObject* a, GameObject* b)
{
    if (!a->hasCollision || !b->hasCollision) return false;
    if (a == b) return false;
    
    float aMinX = a->position.x - a->size.x / 2;
    float aMaxX = a->position.x + a->size.x / 2;
    float aMinY = a->position.y - a->size.y / 2;
    float aMaxY = a->position.y + a->size.y / 2;
    float aMinZ = a->position.z - a->size.z / 2;
    float aMaxZ = a->position.z + a->size.z / 2;
    
    float bMinX = b->position.x - b->size.x / 2;
    float bMaxX = b->position.x + b->size.x / 2;
    float bMinY = b->position.y - b->size.y / 2;
    float bMaxY = b->position.y + b->size.y / 2;
    float bMinZ = b->position.z - b->size.z / 2;
    float bMaxZ = b->position.z + b->size.z / 2;
    
    return (aMaxX > bMinX && aMinX < bMaxX &&
            aMaxY > bMinY && aMinY < bMaxY &&
            aMaxZ > bMinZ && aMinZ < bMaxZ);
}

bool CheckSphereAABBCollision(Vector3 sphereCenter, float sphereRadius, GameObject* box)
{
    Vector3 closestPoint;
    closestPoint.x = fmaxf(box->position.x - box->size.x/2, fminf(sphereCenter.x, box->position.x + box->size.x/2));
    closestPoint.y = fmaxf(box->position.y - box->size.y/2, fminf(sphereCenter.y, box->position.y + box->size.y/2));
    closestPoint.z = fmaxf(box->position.z - box->size.z/2, fminf(sphereCenter.z, box->position.z + box->size.z/2));
    
    float dx = sphereCenter.x - closestPoint.x;
    float dy = sphereCenter.y - closestPoint.y;
    float dz = sphereCenter.z - closestPoint.z;
    float distanceSquared = dx*dx + dy*dy + dz*dz;
    
    return distanceSquared < sphereRadius * sphereRadius;
}

bool CheckSphereCollision(Vector3 c1, float r1, Vector3 c2, float r2)
{
    float dx = c2.x - c1.x;
    float dy = c2.y - c1.y;
    float dz = c2.z - c1.z;
    float distanceSquared = dx*dx + dy*dy + dz*dz;
    float radiusSum = r1 + r2;
    
    return distanceSquared <= radiusSum * radiusSum;
}

static bool CheckPlayerCollision(GameObject* player, GameObject* other)
{
    if (!player || !other || !other->hasCollision) return false;
    
    Vector3 playerCenter = player->position;
    playerCenter.y += player->size.y/2 - player->size.x/2;
    
    float playerRadius = player->size.x / 2;
    
    if (other->type == OBJ_SPHERE)
    {
        return CheckSphereCollision(playerCenter, playerRadius, 
                                   other->position, other->size.x/2);
    }
    else
    {
        return CheckSphereAABBCollision(playerCenter, playerRadius, other);
    }
}

bool CheckCollision(GameObject* a, GameObject* b)
{
    if (!a || !b || !a->hasCollision || !b->hasCollision) return false;
    
    if (a->type == OBJ_SPHERE || b->type == OBJ_SPHERE)
    {
        float radiusA = (a->type == OBJ_SPHERE) ? a->size.x/2 : fmaxf(a->size.x, fmaxf(a->size.y, a->size.z))/2;
        float radiusB = (b->type == OBJ_SPHERE) ? b->size.x/2 : fmaxf(b->size.x, fmaxf(b->size.y, b->size.z))/2;
        
        float dx = b->position.x - a->position.x;
        float dy = b->position.y - a->position.y;
        float dz = b->position.z - a->position.z;
        float distanceSquared = dx*dx + dy*dy + dz*dz;
        float radiusSum = radiusA + radiusB;
        
        return distanceSquared <= radiusSum * radiusSum;
    }
    else
    {
        return CheckAABBCollision(a, b);
    }
}

void ResolvePlayerCollision(GameObject* player, GameObject* other)
{
    if (!player || !other) return;
    
    Vector3 playerCenter = player->position;
    playerCenter.y += player->size.y/2 - player->size.x/2;
    float playerRadius = player->size.x / 2;
    
    Vector3 closestPoint;
    
    if (other->type == OBJ_SPHERE)
    {
        Vector3 delta = (Vector3){other->position.x - playerCenter.x,
                                 other->position.y - playerCenter.y,
                                 other->position.z - playerCenter.z};
        float distance = sqrtf(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);
        if (distance == 0) return;
        
        Vector3 normal = (Vector3){delta.x/distance, delta.y/distance, delta.z/distance};
        closestPoint.x = other->position.x - normal.x * other->size.x/2;
        closestPoint.y = other->position.y - normal.y * other->size.y/2;
        closestPoint.z = other->position.z - normal.z * other->size.z/2;
    }
    else
    {
        closestPoint.x = fmaxf(other->position.x - other->size.x/2, 
                             fminf(playerCenter.x, other->position.x + other->size.x/2));
        closestPoint.y = fmaxf(other->position.y - other->size.y/2, 
                             fminf(playerCenter.y, other->position.y + other->size.y/2));
        closestPoint.z = fmaxf(other->position.z - other->size.z/2, 
                             fminf(playerCenter.z, other->position.z + other->size.z/2));
    }
    
    float dx = playerCenter.x - closestPoint.x;
    float dy = playerCenter.y - closestPoint.y;
    float dz = playerCenter.z - closestPoint.z;
    float collisionDistance = sqrtf(dx*dx + dy*dy + dz*dz);
    
    if (collisionDistance < playerRadius && collisionDistance > 0)
    {
        Vector3 normal = (Vector3){dx/collisionDistance, dy/collisionDistance, dz/collisionDistance};
        
        float overlap = playerRadius - collisionDistance;
        player->position.x += normal.x * overlap * 0.5f;
        player->position.y += normal.y * overlap * 0.5f;
        player->position.z += normal.z * overlap * 0.5f;
        
        if (normal.y > 0.7f)
        {
            player->physics.isGrounded = true;
            player->physics.velocity.y = 0;
        }
        
        float velocityDot = player->physics.velocity.x * normal.x + 
                           player->physics.velocity.z * normal.z;
        if (velocityDot < 0)
        {
            player->physics.velocity.x -= normal.x * velocityDot * 0.8f;
            player->physics.velocity.z -= normal.z * velocityDot * 0.8f;
        }
    }
}

void ResolveCollision(GameObject* a, GameObject* b)
{
    if (a->isTrigger || b->isTrigger) return;
    
    if (a->type == OBJ_PLAYER)
    {
        ResolvePlayerCollision(a, b);
        return;
    }
    if (b->type == OBJ_PLAYER)
    {
        ResolvePlayerCollision(b, a);
        return;
    }
    
    float dx = b->position.x - a->position.x;
    float dy = b->position.y - a->position.y;
    float dz = b->position.z - a->position.z;
    float distance = sqrtf(dx*dx + dy*dy + dz*dz);
    if (distance == 0) return;
    
    float minDistance = (a->size.x + b->size.x) / 4;
    float overlap = minDistance - distance;
    
    if (overlap > 0)
    {
        Vector3 normal = (Vector3){dx/distance, dy/distance, dz/distance};
        
        if (!a->isStatic)
        {
            a->position.x -= normal.x * overlap * 0.5f;
            a->position.y -= normal.y * overlap * 0.5f;
            a->position.z -= normal.z * overlap * 0.5f;
        }
        
        if (!b->isStatic)
        {
            b->position.x += normal.x * overlap * 0.5f;
            b->position.y += normal.y * overlap * 0.5f;
            b->position.z += normal.z * overlap * 0.5f;
        }
    }
}