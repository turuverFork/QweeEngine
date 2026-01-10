//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Shadow System Module
//==================================================================

#ifndef SHADOWS_H
#define SHADOWS_H

#include "raylib.h"
#include "objects.h"
#include <stdbool.h>

typedef enum
{
    SHADOW_SIMPLE,
    SHADOW_SOFT,
    SHADOW_PCF,
    SHADOW_VOLUMETRIC
} ShadowType;

typedef struct
{
    bool enabled;
    ShadowType type;
    Color shadowColor;
    float intensity;
    float softness;
    float maxDistance;
    Vector3 lightDirection;
    bool shadowsEnabled;
    bool receiveShadows;
} ShadowSettings;

void InitShadows(int screenWidth, int screenHeight);
void CloseShadows();
void UpdateShadows();
void RenderShadows();

void SetShadowsEnabled(bool enabled);
void SetShadowType(ShadowType type);
void SetShadowIntensity(float intensity);
void SetShadowSoftness(float softness);
void SetShadowDistance(float distance);
void SetLightDirection(Vector3 direction);
void SetShadowsReceive(bool receive);

void SetObjectCastShadows(GameObject* obj, bool cast);
void SetObjectReceiveShadows(GameObject* obj, bool receive);

void SetDefaultShadows();
void SetSoftShadows();
void SetHardShadows();
void SetDramaticShadows();

ShadowSettings* GetShadowSettings();
RenderTexture2D GetShadowMap();

#endif