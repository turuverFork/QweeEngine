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

#include "engine.h"
#include <string.h>

GameObject* objects[MAX_OBJECTS] = {0};
int objectCount = 0;
GameObject* playerObject = NULL;
Camera3D camera;
PlayerPhysicsSettings playerSettings = {0};
bool wireframeMode = false;

void InitEngine(int screenWidth, int screenHeight, const char* title, bool fullscreen)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    
    if (fullscreen)
    {
        InitWindow(screenWidth, screenHeight, title);
        ToggleFullscreen();
    }
    else
    {
        InitWindow(screenWidth, screenHeight, title);
    }
    
    SetTargetFPS(60);
    playerSettings.walkSpeed = 5.0f;
    playerSettings.runSpeed = 10.0f;
    playerSettings.jumpForce = 12.0f;
    playerSettings.gravity = -25.0f;
    playerSettings.playerHeight = 1.8f;
    playerSettings.playerRadius = 0.3f;
    playerSettings.airControl = 0.3f;
    playerSettings.groundFriction = 0.8f;
    
    InitCamera();
    InitPhysics();  
    DisableCursor();
    
    printf("Qwee Engine initialized successfully!\n");
}

void CloseEngine()
{
    for (int i = 0; i < objectCount; i++)
    {
        if (objects[i])
        {
            DestroyObject(objects[i]);
        }
    }
    
    CloseWindow();
    printf("Qwee Engine closed.\n");
}

void UpdateEngine(float deltaTime)
{
    UpdateDeltaTime();
    UpdateCameraSystem();
    UpdatePhysics(deltaTime);
    UpdatePlayerPhysics(deltaTime);
}

void RenderAll()
{
    BeginDrawing();
    ClearBackground((Color){135, 206, 235, 255});
    
    BeginMode3D(camera);
    
    for (int i = 0; i < objectCount; i++)
    {
        if (objects[i] && objects[i]->isActive)
        {
            DrawObject(objects[i]);
        }
    }
    
    DrawGrid(50, 1.0f);
    
    if (playerObject && wireframeMode)
    {
        Vector3 playerCenter = playerObject->position;
        playerCenter.y += playerObject->size.y/2 - playerObject->size.x/2;
        DrawSphereWires(playerCenter, playerObject->size.x/2, 8, 8, GREEN);
    }
    
    EndMode3D();

    // FPS counter
    DrawFPS(10, 10);
    
    EndDrawing();
}

void ToggleWireframe(bool enabled)
{
    wireframeMode = enabled;
}

GameObject** GetObjects() { return objects; }
int* GetObjectCount() { return &objectCount; }
GameObject** GetPlayerObject() { return &playerObject; }
Camera3D* GetCamera() { return &camera; }
PlayerPhysicsSettings* GetPlayerSettings() { return &playerSettings; }
bool* GetWireframeMode() { return &wireframeMode; }