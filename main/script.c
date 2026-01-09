#include "engine.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    srand((unsigned int)time(NULL));
    
    InitEngine(1280, 720, "Qwee Engine simple script", false);
    
    PlayerPhysicsSettings* playerSettings = GetPlayerSettings();
    if (!playerSettings) {
        printf("ERROR: Failed to get player physics settings!\n");
        return -1;
    }
    
    SetPlayerPhysicsSettings(
        8.0f,    // walkSpeed
        15.0f,   // runSpeed  
        8.0f,    // jumpForce
        -15.0f,  // gravity
        1.8f,    // playerHeight
        0.3f,    // playerRadius
        0.6f,    // airControl
        0.9f     // groundFriction
    );
    
    printf("=== Creating 3D Scene ===\n");
    
    GameObject* player = CreatePlayer("Player", 0.0f, 2.0f, 0.0f, true, true);
    if (!player) {
        printf("ERROR: Failed to create player!\n");
        CloseEngine();
        return -1;
    }
    
    CreatePlane("Ground", 0.0f, 0.0f, 0.0f, 50.0f, 50.0f, (Color){100, 100, 100, 255});
    
    printf("Creating environment...\n");
    
    for (int i = 0; i < 8; i++)
    {
        char name[32];
        snprintf(name, sizeof(name), "WallBlock_%d", i);
        CreateCube(name, -4.0f + i * 1.5f, 1.0f, 8.0f, false, true, 0, (Color){120, 80, 40, 255});
    }
    
    CreateCube("JumpPlatform1", -6.0f, 1.0f, 3.0f, false, true, 0, BLUE);
    CreateCube("JumpPlatform2", -2.0f, 3.0f, 3.0f, false, true, 0, GREEN);
    CreateCube("JumpPlatform3", 2.0f, 5.0f, 3.0f, false, true, 0, RED);
    CreateCube("JumpPlatform4", 6.0f, 7.0f, 3.0f, false, true, 0, YELLOW);
    
    printf("=== Scene created! ===\n");
    printf("Controls:\n");
    printf("  WASD - Move\n");
    printf("  Mouse - Look around\n");
    printf("  SPACE - Jump\n");
    printf("  SHIFT - Run\n");
    printf("  F1 - Toggle wireframe\n");
    printf("  ESC - Exit\n");
    
    bool physicsEnabled = true;
    bool wireframeEnabled = false;
    
    while (!WindowShouldClose())
    {
        float dt = GetDeltaTime();
        if (dt > 0.1f) dt = 0.1f; 
        
    
        if (player && player->isActive)
        {
      
            static int inputCheck = 0;
            if (inputCheck++ % 30 == 0) {
                printf("Input check - W:%d S:%d A:%d D:%d\n", 
                       IsKeyDown(KEY_W), IsKeyDown(KEY_S), 
                       IsKeyDown(KEY_A), IsKeyDown(KEY_D));
            }
            
 
            player->physics.velocity.x = 0;
            player->physics.velocity.z = 0;
            
            if (IsKeyDown(KEY_W))
            {
                player->physics.velocity.z = -5.0f;
                printf("Moving forward\n");
            }
            if (IsKeyDown(KEY_S))
            {
                player->physics.velocity.z = 5.0f;
                printf("Moving backward\n");
            }
            if (IsKeyDown(KEY_A))
            {
                player->physics.velocity.x = -5.0f;
                printf("Moving left\n");
            }
            if (IsKeyDown(KEY_D))
            {
                player->physics.velocity.x = 5.0f;
                printf("Moving right\n");
            }
            
            if (IsKeyDown(KEY_LEFT_SHIFT))
            {
                player->physics.velocity.x *= 2.0f;
                player->physics.velocity.z *= 2.0f;
            }

            if (IsKeyPressed(KEY_SPACE) && player->physics.isGrounded)
            {
                player->physics.velocity.y = playerSettings->jumpForce;
                player->physics.isGrounded = false;
                printf("Jump!\n");
            }

            static int debugCounter = 0;
            if (debugCounter++ % 60 == 0) {
                printf("Player: pos(%.1f, %.1f, %.1f) vel(%.1f, %.1f, %.1f) grounded: %d\n",
                       player->position.x, player->position.y, player->position.z,
                       player->physics.velocity.x, player->physics.velocity.y, player->physics.velocity.z,
                       player->physics.isGrounded);
            }
        }

        if (physicsEnabled)
        {
            UpdatePhysics(dt);
            UpdatePlayerPhysics(dt);
        }

        if (IsKeyPressed(KEY_F1))
        {
            wireframeEnabled = !wireframeEnabled;
            ToggleWireframe(wireframeEnabled);
            printf("Wireframe mode: %s\n", wireframeEnabled ? "ON" : "OFF");
        }
        
        if (IsKeyPressed(KEY_F3))
        {

            if (player)
            {
                player->position = (Vector3){0.0f, 5.0f, 0.0f};
                player->physics.velocity = (Vector3){0, 0, 0};
                printf("Player position reset\n");
            }
        }
        
        if (IsKeyPressed(KEY_R))
        {

            if (player)
            {
                Camera3D* cam = GetCamera();
                Vector3 forward = Vector3Subtract(cam->target, cam->position);
                float length = sqrtf(forward.x*forward.x + forward.y*forward.y + forward.z*forward.z);
                if (length > 0)
                {
                    forward.x /= length;
                    forward.y /= length;
                    forward.z /= length;
                }
                
                float x = player->position.x + forward.x * 2.0f;
                float y = player->position.y + 1.0f;
                float z = player->position.z + forward.z * 2.0f;
                
                char name[32];
                snprintf(name, sizeof(name), "Ball_%d", rand() % 1000);
                
                Color col = (Color){
                    (unsigned char)(rand() % 256),
                    (unsigned char)(rand() % 256),
                    (unsigned char)(rand() % 256),
                    255
                };
                
                GameObject* ball = CreateSphere(name, x, y, z, true, true, col, 0.4f);
                if (ball)
                {
                    ball->physics.velocity.x = forward.x * 15.0f;
                    ball->physics.velocity.y = forward.y * 15.0f + 3.0f;
                    ball->physics.velocity.z = forward.z * 15.0f;
                    ball->physics.bounceFactor = 0.8f;
                    printf("Threw ball: %s\n", name);
                }
            }
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }

        UpdateEngine(dt);
        RenderAll();
    }

    CloseEngine();
    
    return 0;
}