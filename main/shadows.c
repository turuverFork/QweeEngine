//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Simple Shadow System Implementation
//==================================================================

#include "shadows.h"
#include "engine.h"
#include "objects.h"
#include <stdio.h>
#include <math.h>

static ShadowSettings shadowSettings = {
    .enabled = true,
    .type = SHADOW_SIMPLE,
    .shadowColor = {0, 0, 0, 100},
    .intensity = 0.7f,
    .softness = 2.0f,
    .maxDistance = 50.0f,
    .lightDirection = {0.5f, -1.0f, 0.3f},
    .shadowsEnabled = true,
    .receiveShadows = true
};

static RenderTexture2D shadowMap = {0};
static int shadowMapSize = 1024;

void InitShadows(int screenWidth, int screenHeight)
{
    shadowMap = LoadRenderTexture(shadowMapSize, shadowMapSize);

    float length = sqrtf(shadowSettings.lightDirection.x * shadowSettings.lightDirection.x +
                        shadowSettings.lightDirection.y * shadowSettings.lightDirection.y +
                        shadowSettings.lightDirection.z * shadowSettings.lightDirection.z);
    
    if (length > 0)
    {
        shadowSettings.lightDirection.x /= length;
        shadowSettings.lightDirection.y /= length;
        shadowSettings.lightDirection.z /= length;
    }
    
    printf("Shadow system initialized (%dx%d shadow map)\n", shadowMapSize, shadowMapSize);
}

void CloseShadows()
{
    UnloadRenderTexture(shadowMap);
    printf("Shadow system closed\n");
}

void UpdateShadows()
{
    //update
}

void RenderShadows()
{
    if (!shadowSettings.enabled || !shadowSettings.shadowsEnabled) return;
    
    GameObject** objects = GetObjects();
    int* objectCount = GetObjectCount();
    Camera3D* camera = GetCamera();

    Vector3 lightDir = shadowSettings.lightDirection;
    float lightDirLength = sqrtf(lightDir.x*lightDir.x + lightDir.y*lightDir.y + lightDir.z*lightDir.z);
    if (lightDirLength > 0)
    {
        lightDir.x /= lightDirLength;
        lightDir.y /= lightDirLength;
        lightDir.z /= lightDirLength;
    }
    
    for (int i = 0; i < *objectCount; i++)
    {
        GameObject* obj = objects[i];
        if (!obj || !obj->isActive || !obj->isVisible || 
            obj->type == OBJ_PLANE || obj->type == OBJ_PLAYER)
            continue;

        float dx = obj->position.x - camera->position.x;
        float dy = obj->position.y - camera->position.y;
        float dz = obj->position.z - camera->position.z;
        float distance = sqrtf(dx*dx + dy*dy + dz*dz);
        
        if (distance > shadowSettings.maxDistance) continue;

        float groundY = 0.0f; 
        float heightAboveGround = obj->position.y - (obj->size.y / 2) - groundY;
        
        if (heightAboveGround <= 0.01f) continue; 
        
        float lightFactor = -lightDir.y;
        if (lightFactor < 0.01f) lightFactor = 0.01f;
        
        float shadowScale = 1.0f + (heightAboveGround * 0.1f); 
        
        Vector3 shadowPos = obj->position;
        shadowPos.x += lightDir.x * (heightAboveGround / lightFactor) * 0.5f;
        shadowPos.y = groundY + 0.01f; // Slightly above ground
        shadowPos.z += lightDir.z * (heightAboveGround / lightFactor) * 0.5f;

        float distanceFactor = 1.0f - (distance / shadowSettings.maxDistance);
        if (distanceFactor < 0) distanceFactor = 0;
        
        Color shadowCol = shadowSettings.shadowColor;
        shadowCol.a = (unsigned char)(shadowCol.a * shadowSettings.intensity * distanceFactor);

        float shadowSize = obj->size.x * shadowScale;
        
        if (shadowSettings.type == SHADOW_SIMPLE)
        {
            DrawCylinder(shadowPos, shadowSize * 0.5f, shadowSize * 0.5f, 0.02f, 16, shadowCol);
        }
        else if (shadowSettings.type == SHADOW_SOFT)
        {
            for (int layer = 0; layer < 3; layer++)
            {
                float layerScale = 1.0f + (layer * 0.2f);
                Color layerCol = shadowCol;
                layerCol.a = (unsigned char)(shadowCol.a * (0.7f - layer * 0.2f));
                
                DrawCylinder(shadowPos, 
                           shadowSize * 0.5f * layerScale, 
                           shadowSize * 0.5f * layerScale, 
                           0.02f, 16, layerCol);
            }
        }
    }
}

void SetShadowsEnabled(bool enabled)
{
    shadowSettings.enabled = enabled;
    printf("Shadows %s\n", enabled ? "enabled" : "disabled");
}

void SetShadowType(ShadowType type)
{
    shadowSettings.type = type;
    printf("Shadow type set to: %d\n", type);
}

void SetShadowIntensity(float intensity)
{
    shadowSettings.intensity = intensity;
    printf("Shadow intensity set to: %.2f\n", intensity);
}

void SetShadowSoftness(float softness)
{
    shadowSettings.softness = softness;
    printf("Shadow softness set to: %.2f\n", softness);
}

void SetShadowDistance(float distance)
{
    shadowSettings.maxDistance = distance;
    printf("Shadow distance set to: %.1f\n", distance);
}

void SetLightDirection(Vector3 direction)
{
    shadowSettings.lightDirection = direction;
    printf("Light direction set to: (%.2f, %.2f, %.2f)\n", 
           direction.x, direction.y, direction.z);
}

void SetShadowsReceive(bool receive)
{
    shadowSettings.receiveShadows = receive;
    printf("Receive shadows: %s\n", receive ? "yes" : "no");
}

void SetObjectCastShadows(GameObject* obj, bool cast)
{
    if (obj)
    {
        printf("Object %s cast shadows: %s\n", obj->name, cast ? "yes" : "no");
    }
}

void SetObjectReceiveShadows(GameObject* obj, bool receive)
{
    if (obj)
    {
        printf("Object %s receive shadows: %s\n", obj->name, receive ? "yes" : "no");
    }
}

void SetDefaultShadows()
{
    shadowSettings.enabled = true;
    shadowSettings.type = SHADOW_SOFT;
    shadowSettings.intensity = 0.6f;
    shadowSettings.softness = 2.0f;
    shadowSettings.maxDistance = 50.0f;
    shadowSettings.lightDirection = (Vector3){0.5f, -1.0f, 0.3f};
    printf("Default shadows applied\n");
}

void SetSoftShadows()
{
    shadowSettings.type = SHADOW_SOFT;
    shadowSettings.softness = 3.0f;
    shadowSettings.intensity = 0.5f;
    printf("Soft shadows applied\n");
}

void SetHardShadows()
{
    shadowSettings.type = SHADOW_SIMPLE;
    shadowSettings.softness = 0.0f;
    shadowSettings.intensity = 0.8f;
    printf("Hard shadows applied\n");
}

void SetDramaticShadows()
{
    shadowSettings.type = SHADOW_SOFT;
    shadowSettings.intensity = 0.9f;
    shadowSettings.softness = 1.5f;
    shadowSettings.lightDirection = (Vector3){0.8f, -0.5f, 0.2f};
    printf("Dramatic shadows applied\n");
}

ShadowSettings* GetShadowSettings()
{
    return &shadowSettings;
}

RenderTexture2D GetShadowMap()
{
    return shadowMap;
}