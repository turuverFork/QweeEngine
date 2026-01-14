//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Main Engine Header
//==================================================================

#ifndef ENGINE_H
#define ENGINE_H

#include "raylib.h"
#include "physics.h"
#include "objects.h"
#include "camera.h"
#include "utils.h"
#include "vector_math.h"
#include "particles.h"
#include "fog.h"
#include "shadows.h"
#include "audio.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

extern GameObject* objects[MAX_OBJECTS];
extern int objectCount;
extern GameObject* playerObject;
extern Camera3D camera;
extern PlayerPhysicsSettings playerSettings;
extern bool wireframeMode;

extern bool particlesEnabled;
extern bool fogEnabled;
extern bool shadowsEnabled;
extern bool audioEnabled;

void InitEngine(int screenWidth, int screenHeight, const char* title, bool fullscreen);
void CloseEngine();
void UpdateEngine(float deltaTime);
void RenderAll();
void ToggleWireframe(bool enabled);

void ToggleParticles(bool enabled);
void CreatePlayerParticleBurst();
void CreateFireEffect(Vector3 position, float intensity);
void CreateRainEffect(Vector3 areaCenter, Vector3 areaSize);
void CreateSnowEffect(Vector3 areaCenter, Vector3 areaSize);

void ToggleFog(bool enabled);
void SetFogPreset(int preset);

void ToggleShadows(bool enabled);
void SetShadowPreset(int preset);

void ToggleAudio(bool enabled);
void PlayTestSound();
void LoadAndPlayMusic(const char* name, const char* filePath, float volume, bool loop);
void PlaySoundEffect(const char* name, const char* filePath, float volume, float duration);

void Engine_RegisterScene(const char* name, SceneType type, 
                         SceneFunction init, SceneFunction update, 
                         SceneFunction render, SceneFunction close);
void Engine_SwitchScene(const char* name);
const char* Engine_GetCurrentSceneName();
bool Engine_IsCurrentScene2D();
bool Engine_IsCurrentScene3D();

bool GetParticlesEnabled();
bool GetFogEnabled();
bool GetShadowsEnabled();
bool GetAudioEnabled();
ParticleEmitter* GetParticleEmitter(const char* name);
FogSettings* GetEngineFogSettings();
ShadowSettings* GetEngineShadowSettings();
AudioFile* GetAudioFile(const char* name);

GameObject** GetObjects();
int* GetObjectCount();
GameObject** GetPlayerObject();
Camera3D* GetCamera();
PlayerPhysicsSettings* GetPlayerSettings();
bool* GetWireframeMode();

#endif