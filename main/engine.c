//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Engine version: 0.2 | Build: ALPHA-2026
//
//Core Systems:
//  [✓] Physics: Rigidbody dynamics, AABB/Sphere collisions
//  [✓] Rendering: First-person camera, object rendering
//  [✓] Object System: Cubes, spheres, cylinders, pyramids
//  [✓] Camera: First-person controls with mouse look
//  [✓] Input: WASD movement, mouse camera control
//  [✓] Particles: Fire, smoke, rain, snow, sparks, dust
//  [✓] Fog: Linear, exponential, volumetric fog
//  [✓] Shadows: Simple, soft, PCF shadows
//  [✓] Audio: Sound effects, music, 3D positional audio
//  [✓] Scenes: 2D/3D scene system with scene switching
//
///Performance Profile:
//  - Max objects: 500 units
//  - Max particles: 10,000 units
//  - Max audio files: 100 units
//  - Max scenes: 20 units
//  - Target FPS: 60 @ 1280x720
//  - Physics steps: 1000+/sec
//  - Memory footprint: <64KB object pool + <32KB particle pool + <16MB audio pool
//
//Build Configuration:
//  - Platform: Windows/Linux/macOS (via Raylib)
//  - Mode: First-person 3D / 2D scenes
//  - Renderer: Software-accelerated 3D/2D
//
//Controls:
//  W/A/S/D    - Player movement (3D scenes)
//  Mouse      - Camera look (3D scenes)
//  SPACE      - Jump (3D scenes)
//  SHIFT      - Run (3D scenes)
//  F1         - Toggle wireframe
//  F2         - Toggle shadows
//  F3         - Toggle fog
//  F4         - Toggle particles
//  F5         - Toggle audio
//  F6         - Switch to next scene
//  R          - Throw physics ball
//  P          - Create particle burst
//  M          - Play test sound
//  ESC        - Exit engine
// ==================================================================

#include "engine.h"
#include "particles.h"
#include "fog.h"
#include "shadows.h"
#include "audio.h"
#include "scene.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

GameObject* objects[MAX_OBJECTS] = {0};
int objectCount = 0;
GameObject* playerObject = NULL;
Camera3D camera;
PlayerPhysicsSettings playerSettings = {0};
bool wireframeMode = false;

bool particlesEnabled = true;
bool fogEnabled = true;
bool shadowsEnabled = true;
bool audioEnabled = true;

static int currentSceneIndex = 0;
static char sceneNames[MAX_SCENES][32];
static int registeredSceneCount = 0;

void InitEngine(int screenWidth, int screenHeight, const char* title, bool fullscreen)
{
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    
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

    InitParticleSystem();
    InitFog();
    InitShadows(screenWidth, screenHeight);
    InitAudioSystem();
    InitSceneSystem();

    SetFogEnabled(true);
    SetDefaultFog();
    
    SetShadowsEnabled(true);
    SetDefaultShadows();

    DisableCursor();
    
    printf("========================================\n");
    printf("QWEE Engine v0.2 ALPHA-2026\n");
    printf("========================================\n");
    printf("Core Systems Initialized:\n");
    printf("  [✓] Physics System\n");
    printf("  [✓] Object System (500 max)\n");
    printf("  [✓] Camera System\n");
    printf("  [✓] Particle System\n");
    printf("  [✓] Fog System\n");
    printf("  [✓] Shadow System\n");
    printf("  [✓] Audio System\n");
    printf("  [✓] Scene System (20 max)\n");
    printf("========================================\n");
    printf("Controls:\n");
    printf("  WASD - Movement (3D scenes)\n");
    printf("  Mouse - Look around (3D scenes)\n");
    printf("  SPACE - Jump (3D scenes)\n");
    printf("  SHIFT - Run (3D scenes)\n");
    printf("  F1 - Toggle wireframe\n");
    printf("  F2 - Toggle shadows\n");
    printf("  F3 - Toggle fog\n");
    printf("  F4 - Toggle particles\n");
    printf("  F5 - Toggle audio\n");
    printf("  F6 - Switch to next scene\n");
    printf("  R - Throw physics ball (3D scenes)\n");
    printf("  P - Create particle burst\n");
    printf("  M - Play test sound\n");
    printf("  ESC - Exit\n");
    printf("========================================\n");
}

void CloseEngine()
{
    printf("========================================\n");
    printf("Shutting down QWEE Engine...\n");

    printf("Cleaning up scenes...\n");
    Scene* current = GetCurrentScene();
    if (current)
    {
        UnloadScene(current->name);
    }

    printf("Cleaning up objects...\n");
    for (int i = 0; i < objectCount; i++)
    {
        if (objects[i])
        {
            DestroyObject(objects[i]);
        }
    }

    printf("Cleaning up particles...\n");
    CloseParticleSystem();
    
    printf("Cleaning up shadows...\n");
    CloseShadows();
    
    printf("Cleaning up audio...\n");
    CloseAudioSystem();

    CloseWindow();
    
    printf("QWEE Engine shutdown complete.\n");
    printf("========================================\n");
}

void UpdateEngine(float deltaTime)
{
    UpdateDeltaTime();

    if (Engine_IsCurrentScene3D())
    {
        UpdateCameraSystem();
        UpdatePhysics(deltaTime);
        UpdatePlayerPhysics(deltaTime);
    }
    
    if (particlesEnabled)
    {
        UpdateParticles(deltaTime);
    }

    if (shadowsEnabled && Engine_IsCurrentScene3D())
    {
        UpdateShadows();
    }
    
    if (audioEnabled)
    {
        UpdateAudio();
    }

    UpdateCurrentScene();
}

void RenderAll()
{
    BeginDrawing();
    
    Scene* scene = GetCurrentScene();
    
    if (scene && scene->type == SCENE_2D)
    {
        ClearBackground(RAYWHITE);
        RenderCurrentScene();
    }
    else
    {
        FogSettings* fog = GetFogSettings();
        if (fogEnabled && fog->enabled && fog->skyAffected)
        {
            Color skyColor = (Color){135, 206, 235, 255};
            Color finalSkyColor = {
                (unsigned char)(skyColor.r * 0.7f + fog->color.r * 0.3f),
                (unsigned char)(skyColor.g * 0.7f + fog->color.g * 0.3f),
                (unsigned char)(skyColor.b * 0.7f + fog->color.b * 0.3f),
                255
            };
            ClearBackground(finalSkyColor);
        }
        else
        {
            ClearBackground((Color){135, 206, 235, 255});
        }
        
        BeginMode3D(camera);

        if (fogEnabled)
        {
            ApplyFog();
        }

        for (int i = 0; i < objectCount; i++)
        {
            if (objects[i] && objects[i]->isActive)
            {
                DrawObject(objects[i]);
            }
        }
        
        if (fogEnabled && fog->enabled)
        {
            for (int i = -25; i <= 25; i++)
            {
                for (int j = -25; j <= 25; j++)
                {
                    Vector3 lineStart1 = {i, 0.01f, -25};
                    Vector3 lineEnd1 = {i, 0.01f, 25};
                    Vector3 lineStart2 = {-25, 0.01f, j};
                    Vector3 lineEnd2 = {25, 0.01f, j};

                    float dist1 = Vector3Length(Vector3Subtract(camera.position, lineStart1));
                    float dist2 = Vector3Length(Vector3Subtract(camera.position, lineStart2));

                    Color gridColor = LIGHTGRAY;
                    if (fogEnabled && fog->enabled)
                    {
                        float fogFactor1 = 1.0f;
                        float fogFactor2 = 1.0f;
                        
                        if (fog->type == FOG_LINEAR)
                        {
                            fogFactor1 = (fog->endDistance - dist1) / (fog->endDistance - fog->startDistance);
                            fogFactor2 = (fog->endDistance - dist2) / (fog->endDistance - fog->startDistance);
                        }
                        else if (fog->type == FOG_EXPONENTIAL)
                        {
                            fogFactor1 = expf(-fog->density * dist1);
                            fogFactor2 = expf(-fog->density * dist2);
                        }
                        else if (fog->type == FOG_EXPONENTIAL_SQUARED)
                        {
                            fogFactor1 = expf(-powf(fog->density * dist1, 2));
                            fogFactor2 = expf(-powf(fog->density * dist2, 2));
                        }
                        
                        fogFactor1 = (fogFactor1 < 0) ? 0 : (fogFactor1 > 1) ? 1 : fogFactor1;
                        fogFactor2 = (fogFactor2 < 0) ? 0 : (fogFactor2 > 1) ? 1 : fogFactor2;
                        
                        gridColor.a = (unsigned char)(200 * fogFactor1);
                    }

                    if (gridColor.a > 10)
                    {
                        DrawLine3D(lineStart1, lineEnd1, gridColor);
                        DrawLine3D(lineStart2, lineEnd2, gridColor);
                    }
                }
            }
        }
        else
        {
            DrawGrid(50, 1.0f);
        }

        if (shadowsEnabled)
        {
            RenderShadows();
        }

        if (particlesEnabled)
        {
            RenderParticles();
        }

        if (playerObject && wireframeMode && Engine_IsCurrentScene3D())
        {
            Vector3 playerCenter = playerObject->position;
            playerCenter.y += playerObject->size.y/2 - playerObject->size.x/2;
            DrawSphereWires(playerCenter, playerObject->size.x/2, 8, 8, GREEN);
        }
        
        EndMode3D();

        RenderCurrentScene();
    }

    DrawFPS(10, 10);

    int yPos = 40;
    DrawText(TextFormat("Scene: %s (%s)", Engine_GetCurrentSceneName(), 
                       Engine_IsCurrentScene2D() ? "2D" : "3D"), 10, yPos, 20, WHITE);
    yPos += 25;
    
    DrawText(TextFormat("Objects: %d/%d", objectCount, MAX_OBJECTS), 10, yPos, 20, WHITE);
    yPos += 25;
    
    DrawText(TextFormat("Wireframe: %s", wireframeMode ? "ON" : "OFF"), 10, yPos, 20, wireframeMode ? YELLOW : WHITE);
    yPos += 25;
    
    if (Engine_IsCurrentScene3D())
    {
        DrawText(TextFormat("Shadows: %s", shadowsEnabled ? "ON" : "OFF"), 10, yPos, 20, shadowsEnabled ? GREEN : WHITE);
        yPos += 25;
        
        DrawText(TextFormat("Fog: %s", fogEnabled ? "ON" : "OFF"), 10, yPos, 20, fogEnabled ? SKYBLUE : WHITE);
        yPos += 25;
    }
    
    DrawText(TextFormat("Particles: %s", particlesEnabled ? "ON" : "OFF"), 10, yPos, 20, particlesEnabled ? ORANGE : WHITE);
    yPos += 25;
    
    DrawText(TextFormat("Audio: %s", audioEnabled ? "ON" : "OFF"), 10, yPos, 20, audioEnabled ? PURPLE : WHITE);
    yPos += 25;

    if (playerObject && Engine_IsCurrentScene3D())
    {
        DrawText(TextFormat("Pos: (%.1f, %.1f, %.1f)", 
            playerObject->position.x, playerObject->position.y, playerObject->position.z), 
            10, GetScreenHeight() - 60, 20, LIGHTGRAY);
        DrawText(TextFormat("Grounded: %s", playerObject->physics.isGrounded ? "YES" : "NO"), 
            10, GetScreenHeight() - 35, 20, playerObject->physics.isGrounded ? GREEN : RED);
    }
    
    const char* controls = Engine_IsCurrentScene3D() ?
        "F1:Wireframe F2:Shadows F3:Fog F4:Particles F5:Audio F6:NextScene R:Throw P:Burst M:Sound" :
        "F4:Particles F5:Audio F6:NextScene P:Burst M:Sound";
    
    DrawText(controls, GetScreenWidth() - 500, 10, 20, LIGHTGRAY);
    
    EndDrawing();
}

void ToggleWireframe(bool enabled)
{
    wireframeMode = enabled;
}

void ToggleParticles(bool enabled)
{
    particlesEnabled = enabled;
    if (enabled)
    {
        printf("Particle system enabled\n");
    }
    else
    {
        printf("Particle system disabled\n");
    }
}

void ToggleFog(bool enabled)
{
    fogEnabled = enabled;
    SetFogEnabled(enabled);
}

void ToggleShadows(bool enabled)
{
    shadowsEnabled = enabled;
    SetShadowsEnabled(enabled);
}

void ToggleAudio(bool enabled)
{
    audioEnabled = enabled;
    if (enabled)
    {
        SetEngineMasterVolume(1.0f);
        printf("Audio system enabled\n");
    }
    else
    {
        SetEngineMasterVolume(0.0f);
        printf("Audio system disabled\n");
    }
}

void PlayTestSound()
{
    if (FileExists("test.wav"))
    {
        PlaySoundEffect("test", "test.wav", 1.0f, 1.0f);
    }
    else
    {
        printf("Test sound file not found: test.wav\n");
    }
}

void LoadAndPlayMusic(const char* name, const char* filePath, float volume, bool loop)
{
    if (!audioEnabled) return;
    
    if (FileExists(filePath))
    {
        AudioFile* music = LoadAudio(name, filePath, MUSIC_TRACK, loop);
        if (music)
        {
            PlayAudio(music, volume);
        }
    }
    else
    {
        printf("Music file not found: %s\n", filePath);
    }
}

void PlaySoundEffect(const char* name, const char* filePath, float volume, float duration)
{
    if (!audioEnabled) return;
    
    if (FileExists(filePath))
    {
        AudioFile* sound = LoadAudio(name, filePath, SOUND_EFFECT, false);
        if (sound)
        {
            if (duration > 0)
            {
                PlayAudioTimed(sound, volume, duration);
            }
            else
            {
                PlayAudio(sound, volume);
            }
        }
    }
    else
    {
        printf("Sound file not found: %s\n", filePath);
    }
}

void CreatePlayerParticleBurst()
{
    if (!playerObject) return;
    
    Vector3 playerPos = playerObject->position;
    playerPos.y += 1.0f; 

    ParticleEmitter* sparks = CreateSparkEmitter(playerPos, 30);
    if (sparks)
    {
        sparks->startColor = (Color){255, 255, 100, 255};
        sparks->endColor = (Color){255, 100, 0, 0};
        printf("Created particle burst at player position\n");
    }
}

void CreateFireEffect(Vector3 position, float intensity)
{
    ParticleEmitter* fire = CreateFireEmitter(position, intensity);
    if (fire)
    {
        Vector3 smokePos = position;
        smokePos.y += 0.5f;
        CreateSmokeEmitter(smokePos, intensity * 0.5f);
        printf("Created fire effect at (%.1f, %.1f, %.1f)\n", 
               position.x, position.y, position.z);
    }
}

void CreateRainEffect(Vector3 areaCenter, Vector3 areaSize)
{
    ParticleEmitter* rain = CreateRainEmitter(areaCenter, areaSize);
    if (rain)
    {
        printf("Created rain effect in area (%.1f, %.1f, %.1f)\n", 
               areaSize.x, areaSize.y, areaSize.z);
    }
}

void CreateSnowEffect(Vector3 areaCenter, Vector3 areaSize)
{
    ParticleEmitter* snow = CreateSnowEmitter(areaCenter, areaSize);
    if (snow)
    {
        printf("Created snow effect in area (%.1f, %.1f, %.1f)\n", 
               areaSize.x, areaSize.y, areaSize.z);
    }
}

void SetFogPreset(int preset)
{
    switch(preset)
    {
        case 0: SetDefaultFog(); break;
        case 1: SetMountainFog(); break;
        case 2: SetForestFog(); break;
        case 3: SetDungeonFog(); break;
        case 4: SetVolumetricFog(0.015f); break;
        default: SetDefaultFog(); break;
    }
}

void SetShadowPreset(int preset)
{
    switch(preset)
    {
        case 0: SetDefaultShadows(); break;
        case 1: SetSoftShadows(); break;
        case 2: SetHardShadows(); break;
        case 3: SetDramaticShadows(); break;
        default: SetDefaultShadows(); break;
    }
}


void Engine_RegisterScene(const char* name, SceneType type, 
                         SceneFunction init, SceneFunction update, 
                         SceneFunction render, SceneFunction close)
{
    RegisterScene(name, type, init, update, render, close);

    if (registeredSceneCount < MAX_SCENES)
    {
        strcpy(sceneNames[registeredSceneCount++], name);
    }
}

void Engine_SwitchScene(const char* name)
{
    SwitchScene(name);
}

const char* Engine_GetCurrentSceneName()
{
    return GetCurrentSceneName();
}

bool Engine_IsCurrentScene2D()
{
    return IsScene2D();
}

bool Engine_IsCurrentScene3D()
{
    return IsScene3D();
}

static void SwitchToNextScene()
{
    if (registeredSceneCount == 0) return;
    
    Scene* current = GetCurrentScene();
    int nextIndex = 0;
    
    if (current)
    {
        for (int i = 0; i < registeredSceneCount; i++)
        {
            if (strcmp(sceneNames[i], current->name) == 0)
            {
                nextIndex = (i + 1) % registeredSceneCount;
                break;
            }
        }
    }
    
    Engine_SwitchScene(sceneNames[nextIndex]);
}

bool GetParticlesEnabled() { return particlesEnabled; }
bool GetFogEnabled() { return fogEnabled; }
bool GetShadowsEnabled() { return shadowsEnabled; }
bool GetAudioEnabled() { return audioEnabled; }

ParticleEmitter* GetParticleEmitter(const char* name)
{
    return FindParticleEmitter(name);
}

FogSettings* GetEngineFogSettings()
{
    return GetFogSettings();
}

ShadowSettings* GetEngineShadowSettings()
{
    return GetShadowSettings();
}

AudioFile* GetAudioFile(const char* name)
{
    return FindAudio(name);
}

GameObject** GetObjects() { return objects; }
int* GetObjectCount() { return &objectCount; }
GameObject** GetPlayerObject() { return &playerObject; }
Camera3D* GetCamera() { return &camera; }
PlayerPhysicsSettings* GetPlayerSettings() { return &playerSettings; }
bool* GetWireframeMode() { return &wireframeMode; }