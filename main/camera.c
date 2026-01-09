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

#include "camera.h"
#include "engine.h"
#include "vector_math.h"

static float cameraHeight = 1.6f;
static float cameraYaw = 0.0f;
static float cameraPitch = 0.0f;

void InitCamera()
{
    Camera3D* cam = GetCamera();
    cam->position = (Vector3){ 0.0f, cameraHeight, 0.0f };
    cam->target = (Vector3){ 0.0f, cameraHeight, 1.0f };
    cam->up = (Vector3){ 0.0f, 1.0f, 0.0f };
    cam->fovy = 70.0f;
    cam->projection = CAMERA_PERSPECTIVE;
}

void UpdateCameraSystem()
{
    GameObject** playerObj = GetPlayerObject();
    Camera3D* cam = GetCamera();
    
    if (!*playerObj) return;
    
    Vector2 mouseDelta = GetMouseDelta();
    float mouseSensitivity = 0.002f;
    
    cameraYaw += mouseDelta.x * mouseSensitivity;
    cameraPitch -= mouseDelta.y * mouseSensitivity;
    
    if (cameraPitch > 1.5f) cameraPitch = 1.5f;
    if (cameraPitch < -1.5f) cameraPitch = -1.5f;
    
    Vector3 direction;
    direction.x = cosf(cameraYaw) * cosf(cameraPitch);
    direction.y = sinf(cameraPitch);
    direction.z = sinf(cameraYaw) * cosf(cameraPitch);
    
    cam->position.x = (*playerObj)->position.x;
    cam->position.y = (*playerObj)->position.y + cameraHeight;
    cam->position.z = (*playerObj)->position.z;
    
    cam->target.x = cam->position.x + direction.x;
    cam->target.y = cam->position.y + direction.y;
    cam->target.z = cam->position.z + direction.z;
}

MovementVectors CalculateMovementDirection()
{
    MovementVectors result;
    Camera3D* cam = GetCamera();
    
    Vector3 forward = Vector3Subtract(cam->target, cam->position);
    forward.y = 0;
    
    float length = sqrtf(forward.x*forward.x + forward.z*forward.z);
    if (length > 0)
    {
        forward.x /= length;
        forward.z /= length;
    }
    
    Vector3 right = Vector3CrossProduct(forward, (Vector3){0, 1, 0});
    
    length = sqrtf(right.x*right.x + right.z*right.z);
    if (length > 0)
    {
        right.x /= length;
        right.z /= length;
    }
    
    result.forward = forward;
    result.right = right;
    
    return result;
}

Vector3 GetMovementVector()
{
    Vector3 move = {0, 0, 0};
    
    if (IsKeyDown(KEY_W)) move.z -= 1.0f;
    if (IsKeyDown(KEY_S)) move.z += 1.0f;
    if (IsKeyDown(KEY_A)) move.x -= 1.0f;
    if (IsKeyDown(KEY_D)) move.x += 1.0f;
    
    float length = sqrtf(move.x*move.x + move.z*move.z);
    if (length > 0)
    {
        move.x /= length;
        move.z /= length;
    }
    
    return move;
}