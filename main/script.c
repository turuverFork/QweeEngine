#include "engine.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(void)
{
    srand((unsigned int)time(NULL));

    InitEngine(1280, 720, "QWEE Engine - Particle & Fog Demo", false);

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
    
    printf("========================================\n");
    printf("QWEE Engine - Particle & Fog Demo\n");
    printf("========================================\n");
 
    GameObject* player = CreatePlayer("Player", 0.0f, 5.0f, 0.0f, true, true);
    if (!player) {
        printf("ERROR: Failed to create player!\n");
        CloseEngine();
        return -1;
    }

    CreatePlane("Ground", 0.0f, 0.0f, 0.0f, 100.0f, 100.0f, (Color){60, 80, 50, 255});
    
    printf("Creating scene with particles and fog...\n");

    printf("\n=== Setting up Fog Systems ===\n");

    SetFogEnabled(true);
    
    printf("1. Forest Fog (dense, greenish)\n");
    SetFogPreset(2);
    
    /*
    SetFogEnabled(true);
    SetFogType(FOG_EXPONENTIAL);
    SetFogColor((Color){100, 130, 100, 255});
    SetFogDensity(0.01f);
    SetFogDistance(5.0f, 40.0f);
    SetFogSkyAffected(false);
    */
    
    printf("\n=== Creating Scene Objects ===\n");

    CreateCube("Platform1", -8.0f, 2.0f, 0.0f, false, true, 0, BLUE);
    CreateCube("Platform2", -4.0f, 4.0f, 0.0f, false, true, 0, GREEN);
    CreateCube("Platform3", 0.0f, 6.0f, 0.0f, false, true, 0, RED);
    CreateCube("Platform4", 4.0f, 8.0f, 0.0f, false, true, 0, YELLOW);
    CreateCube("Platform5", 8.0f, 10.0f, 0.0f, false, true, 0, PURPLE);

    for (int i = -3; i <= 3; i++) 
    {
        char name[32];
        snprintf(name, sizeof(name), "Pillar_%d", i+4);
        CreateCylinder(name, i * 5.0f, 3.0f, -10.0f, false, true, 
                      (Color){180, 160, 140, 255}, 0.8f, 6.0f);
    }

    CreatePyramid("Pyramid1", -12.0f, 2.0f, -8.0f, false, true, 0, (Color){220, 180, 60, 255});
    CreatePyramid("Pyramid2", 12.0f, 2.0f, -8.0f, false, true, 0, (Color){220, 180, 60, 255});

    for (int i = 0; i < 5; i++) {
        char name[32];
        snprintf(name, sizeof(name), "FloatingSphere_%d", i);
        float x = -15.0f + i * 7.5f;
        float y = 8.0f + sinf(i * 0.8f) * 2.0f;
        CreateSphere(name, x, y, 5.0f, true, true, 
                    (Color){rand()%256, rand()%256, rand()%256, 255}, 1.0f);
    }

    printf("\n=== Creating Particle Systems ===\n");

    printf("1. Campfire in the center\n");
    CreateFireEffect((Vector3){0.0f, 0.5f, -5.0f}, 1.2f);

    printf("2. Torches around the area\n");
    Vector3 torchPositions[] = {
        {-10.0f, 1.0f, -5.0f},
        {10.0f, 1.0f, -5.0f},
        {-5.0f, 1.0f, 10.0f},
        {5.0f, 1.0f, 10.0f}
    };
    
    for (int i = 0; i < 4; i++) 
    {
        CreateFireEffect(torchPositions[i], 0.6f);
        Vector3 smokePos = torchPositions[i];
        smokePos.y += 1.5f;
        CreateSmokeEmitter(smokePos, 0.3f);
    }

    printf("3. Magic fountain\n");
    ParticleEmitter* fountain = CreateParticleEmitter("MagicFountain", (Vector3){-15.0f, 2.0f, 15.0f}, PARTICLE_MAGIC);
    if (fountain) {
        fountain->maxParticles = 200;
        fountain->particlesPerSecond = 30;
        fountain->particleLifetime = 3.0f;
        fountain->minSize = 0.1f;
        fountain->maxSize = 0.3f;
        fountain->startColor = (Color){100, 200, 255, 200};
        fountain->endColor = (Color){50, 100, 255, 0};
        fountain->velocityMin = (Vector3){-0.3f, 2.0f, -0.3f};
        fountain->velocityMax = (Vector3){0.3f, 5.0f, 0.3f};
        fountain->gravity = (Vector3){0, -2.0f, 0};
        printf("   Magic fountain created at (-15, 2, 15)\n");
    }
    
    printf("4. Dust cloud\n");
    CreateDustEmitter((Vector3){20.0f, 1.0f, 20.0f}, 5.0f);

    printf("5. Snowfall effect\n");
    CreateSnowEffect((Vector3){0, 25, 0}, (Vector3){80, 25, 80});
    
    
    printf("\n=== Scene Ready! ===\n");
    printf("Controls:\n");
    printf("  WASD      - Move player\n");
    printf("  Mouse     - Look around\n");
    printf("  SPACE     - Jump\n");
    printf("  SHIFT     - Run\n");
    printf("  F1        - Toggle wireframe mode\n");
    printf("  F2        - Toggle shadows\n");
    printf("  F3        - Toggle fog (current: ON)\n");
    printf("  F4        - Toggle particles (current: ON)\n");
    printf("  F5-F9     - Change fog type\n");
    printf("  R         - Throw physics ball\n");
    printf("  P         - Create particle burst at player\n");
    printf("  C         - Clear all particles\n");
    printf("  1-5       - Switch between fog presets\n");
    printf("  F10       - Reset player position\n");
    printf("  ESC       - Exit\n");
    printf("\nFog Presets:\n");
    printf("  1 - Default Fog\n");
    printf("  2 - Mountain Fog\n");
    printf("  3 - Forest Fog (current)\n");
    printf("  4 - Dungeon Fog\n");
    printf("  5 - Volumetric Fog\n");
    printf("========================================\n");

    bool wireframeEnabled = false;
    int currentFogPreset = 3; 

    while (!WindowShouldClose())
    {
        float dt = GetDeltaTime();
        if (dt > 0.1f) dt = 0.1f;

        if (player && player->isActive)
        {
            MovementVectors moveDirs = CalculateMovementDirection();
            Vector3 moveInput = GetMovementVector();
            Vector3 moveVec = {0};
            moveVec.x = moveDirs.forward.x * moveInput.z + moveDirs.right.x * moveInput.x;
            moveVec.z = moveDirs.forward.z * moveInput.z + moveDirs.right.z * moveInput.x;

            float moveLength = sqrtf(moveVec.x*moveVec.x + moveVec.z*moveVec.z);
            if (moveLength > 0)
            {
                moveVec.x /= moveLength;
                moveVec.z /= moveLength;
            }

            float speed = playerSettings->walkSpeed;
            if (IsKeyDown(KEY_LEFT_SHIFT))
            {
                speed = playerSettings->runSpeed;
            }
            
            player->physics.velocity.x = moveVec.x * speed;
            player->physics.velocity.z = moveVec.z * speed;

            if (IsKeyPressed(KEY_SPACE) && player->physics.isGrounded)
            {
                player->physics.velocity.y = playerSettings->jumpForce;
                player->physics.isGrounded = false;

                Vector3 jumpParticlePos = player->position;
                jumpParticlePos.y += 0.5f;
                CreateSparkEmitter(jumpParticlePos, 15);
                printf("Jump! Created jump particles\n");
            }

            static int debugCounter = 0;
            if (debugCounter++ % 60 == 0) {
                printf("Player: pos(%.1f, %.1f, %.1f) | Grounded: %s\n",
                       player->position.x, player->position.y, player->position.z,
                       player->physics.isGrounded ? "YES" : "NO");
            }
        }
        
        if (IsKeyPressed(KEY_F1))
        {
            wireframeEnabled = !wireframeEnabled;
            ToggleWireframe(wireframeEnabled);
            printf("Wireframe mode: %s\n", wireframeEnabled ? "ON" : "OFF");
        }

        if (IsKeyPressed(KEY_F2))
        {
            ToggleShadows(!GetShadowsEnabled());
        }

        if (IsKeyPressed(KEY_F3))
        {
            ToggleFog(!GetFogEnabled());
        }

        if (IsKeyPressed(KEY_F4))
        {
            ToggleParticles(!GetParticlesEnabled());
        }
 
        if (IsKeyPressed(KEY_F5)) {
            SetFogType(FOG_LINEAR);
            printf("Fog type: Linear\n");
        }
        if (IsKeyPressed(KEY_F6)) {
            SetFogType(FOG_EXPONENTIAL);
            printf("Fog type: Exponential\n");
        }
        if (IsKeyPressed(KEY_F7)) {
            SetFogType(FOG_EXPONENTIAL_SQUARED);
            printf("Fog type: Exponential Squared\n");
        }

        if (IsKeyPressed(KEY_ONE)) {
            SetFogPreset(0);
            currentFogPreset = 0;
            printf("Fog preset: Default\n");
        }
        if (IsKeyPressed(KEY_TWO)) {
            SetFogPreset(1);
            currentFogPreset = 1;
            printf("Fog preset: Mountain\n");
        }
        if (IsKeyPressed(KEY_THREE)) {
            SetFogPreset(2);
            currentFogPreset = 2;
            printf("Fog preset: Forest\n");
        }
        if (IsKeyPressed(KEY_FOUR)) {
            SetFogPreset(3);
            currentFogPreset = 3;
            printf("Fog preset: Dungeon\n");
        }
        if (IsKeyPressed(KEY_FIVE)) {
            SetFogPreset(4);
            currentFogPreset = 4;
            printf("Fog preset: Volumetric\n");
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
                
                Color ballColor = (Color){
                    (unsigned char)(rand() % 256),
                    (unsigned char)(rand() % 256),
                    (unsigned char)(rand() % 256),
                    255
                };
                
                GameObject* ball = CreateSphere(name, x, y, z, true, true, ballColor, 0.5f);
                if (ball)
                {
                    ball->physics.velocity.x = forward.x * 20.0f;
                    ball->physics.velocity.y = forward.y * 20.0f + 5.0f;
                    ball->physics.velocity.z = forward.z * 20.0f;
                    ball->physics.bounceFactor = 0.7f;
 
                    CreateSparkEmitter((Vector3){x, y, z}, 20);
                    
                    printf("Threw ball: %s with particle trail\n", name);

                    char emitterName[32];
                    snprintf(emitterName, sizeof(emitterName), "Trail_%s", name);
                    ParticleEmitter* trail = CreateParticleEmitter(emitterName, (Vector3){x, y, z}, PARTICLE_SPARK);
                    if (trail) {
                        trail->maxParticles = 50;
                        trail->particlesPerSecond = 30;
                        trail->particleLifetime = 1.0f;
                        trail->minSize = 0.05f;
                        trail->maxSize = 0.15f;
                        trail->startColor = ballColor;
                        trail->endColor = (Color){ballColor.r, ballColor.g, ballColor.b, 0};
                        trail->velocityMin = (Vector3){-0.5f, -0.5f, -0.5f};
                        trail->velocityMax = (Vector3){0.5f, 0.5f, 0.5f};
                        trail->gravity = (Vector3){0, -1.0f, 0};
                        trail->loop = true;
                    }
                }
            }
        }

        if (IsKeyPressed(KEY_P))
        {
            CreatePlayerParticleBurst();
            printf("Created particle burst at player\n");
        }

        if (IsKeyPressed(KEY_C))
        {
            ClearAllParticles();
            printf("Cleared all particles\n");
        }
 
        if (IsKeyPressed(KEY_F10))
        {
            if (player)
            {
                player->position = (Vector3){0.0f, 10.0f, 0.0f};
                player->physics.velocity = (Vector3){0, 0, 0};
                printf("Player position reset\n");

                CreateSparkEmitter(player->position, 50);
            }
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_SPACE))
        {
            if (currentFogPreset == 3) 
            { 
                SetFogPreset(1);
                currentFogPreset = 1;
                printf("Weather changed: Mountain fog\n");
            } 
            else 
            {
                SetFogPreset(3);
                currentFogPreset = 3;
                printf("Weather changed: Forest fog\n");
            }
        }

        UpdateEngine(dt);

        RenderAll();

        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }
    }

    CloseEngine();
    
    return 0;
}