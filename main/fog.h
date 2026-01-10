//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Fog System Module
//==================================================================

#ifndef FOG_H
#define FOG_H

#include "raylib.h"
#include <stdbool.h>

typedef enum
{
    FOG_LINEAR,
    FOG_EXPONENTIAL,
    FOG_EXPONENTIAL_SQUARED
} FogType;

typedef struct
{
    bool enabled;
    FogType type;
    Color color;
    float density;
    float startDistance;
    float endDistance;
    bool skyAffected;
} FogSettings;

void InitFog();
void SetFogEnabled(bool enabled);
void SetFogType(FogType type);
void SetFogColor(Color color);
void SetFogDensity(float density);
void SetFogDistance(float start, float end);
void SetFogSkyAffected(bool affected);

void SetDefaultFog();
void SetMountainFog();
void SetForestFog();
void SetDungeonFog();
void SetVolumetricFog(float density);

void ApplyFog();
FogSettings* GetFogSettings();

#endif