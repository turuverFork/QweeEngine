//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Fog System Implementation
//==================================================================

#include "fog.h"
#include "engine.h"
#include <stdio.h>

static FogSettings fogSettings = {
    .enabled = false,
    .type = FOG_EXPONENTIAL,
    .color = {200, 200, 210, 255},
    .density = 0.01f,
    .startDistance = 10.0f,
    .endDistance = 50.0f,
    .skyAffected = true
};

void InitFog()
{
    printf("Fog system initialized\n");
}

void SetFogEnabled(bool enabled)
{
    fogSettings.enabled = enabled;
    printf("Fog %s\n", enabled ? "enabled" : "disabled");
}

void SetFogType(FogType type)
{
    fogSettings.type = type;
    printf("Fog type set to: %d\n", type);
}

void SetFogColor(Color color)
{
    fogSettings.color = color;
    printf("Fog color set to: R:%d G:%d B:%d\n", color.r, color.g, color.b);
}

void SetFogDensity(float density)
{
    fogSettings.density = density;
    printf("Fog density set to: %.3f\n", density);
}

void SetFogDistance(float start, float end)
{
    fogSettings.startDistance = start;
    fogSettings.endDistance = end;
    printf("Fog distance: start=%.1f, end=%.1f\n", start, end);
}

void SetFogSkyAffected(bool affected)
{
    fogSettings.skyAffected = affected;
    printf("Fog sky affected: %s\n", affected ? "yes" : "no");
}

void ApplyFog()
{
    if (!fogSettings.enabled) return;
}

void SetDefaultFog()
{
    fogSettings.enabled = true;
    fogSettings.type = FOG_EXPONENTIAL;
    fogSettings.color = (Color){200, 200, 210, 255};
    fogSettings.density = 0.005f;
    fogSettings.startDistance = 20.0f;
    fogSettings.endDistance = 100.0f;
    fogSettings.skyAffected = true;
    printf("Default fog applied\n");
}

void SetMountainFog()
{
    fogSettings.enabled = true;
    fogSettings.type = FOG_EXPONENTIAL_SQUARED;
    fogSettings.color = (Color){180, 200, 220, 255};
    fogSettings.density = 0.003f;
    fogSettings.startDistance = 30.0f;
    fogSettings.endDistance = 150.0f;
    fogSettings.skyAffected = true;
    printf("Mountain fog applied\n");
}

void SetForestFog()
{
    fogSettings.enabled = true;
    fogSettings.type = FOG_LINEAR;
    fogSettings.color = (Color){100, 130, 100, 255};
    fogSettings.density = 0.01f;
    fogSettings.startDistance = 5.0f;
    fogSettings.endDistance = 40.0f;
    fogSettings.skyAffected = false;
    printf("Forest fog applied\n");
}

void SetDungeonFog()
{
    fogSettings.enabled = true;
    fogSettings.type = FOG_EXPONENTIAL;
    fogSettings.color = (Color){50, 50, 60, 255};
    fogSettings.density = 0.02f;
    fogSettings.startDistance = 3.0f;
    fogSettings.endDistance = 20.0f;
    fogSettings.skyAffected = false;
    printf("Dungeon fog applied\n");
}

void SetVolumetricFog(float density)
{
    fogSettings.enabled = true;
    fogSettings.type = FOG_EXPONENTIAL_SQUARED;
    fogSettings.color = (Color){180, 180, 200, 255};
    fogSettings.density = density;
    fogSettings.startDistance = 5.0f;
    fogSettings.endDistance = 50.0f;
    fogSettings.skyAffected = true;
    printf("Volumetric fog applied (density: %.3f)\n", density);
}

FogSettings* GetFogSettings()
{
    return &fogSettings;
}