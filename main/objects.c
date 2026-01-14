//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Object System Implementation (Simplified)
//==================================================================

#include "objects.h"
#include "engine.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static struct {
    char name[64];
    Texture2D texture;
    int refCount;
} texturePool[MAX_TEXTURES];
static int textureCount = 0;

Texture2D LoadGameTexture(const char* path, const char* textureName)
{
    for (int i = 0; i < textureCount; i++) {
        if (strcmp(texturePool[i].name, textureName) == 0) {
            texturePool[i].refCount++;
            printf("Reusing texture: %s (refs: %d)\n", textureName, texturePool[i].refCount);
            return texturePool[i].texture;
        }
    }
    
    if (textureCount >= MAX_TEXTURES) {
        printf("Warning: Maximum texture limit reached!\n");
        return (Texture2D){0};
    }
    
    if (FileExists(path)) {
        Texture2D tex = LoadTexture(path);
        strcpy(texturePool[textureCount].name, textureName);
        texturePool[textureCount].texture = tex;
        texturePool[textureCount].refCount = 1;
        textureCount++;
        
        printf("Loaded texture: %s from %s (%dx%d)\n", 
               textureName, path, tex.width, tex.height);
        return tex;
    } else {
        printf("Warning: Texture file not found: %s\n", path);
        return (Texture2D){0};
    }
}

void UnloadGameTexture(const char* textureName)
{
    for (int i = 0; i < textureCount; i++) {
        if (strcmp(texturePool[i].name, textureName) == 0) {
            texturePool[i].refCount--;
            if (texturePool[i].refCount <= 0) {
                UnloadTexture(texturePool[i].texture);
                printf("Unloaded texture: %s\n", textureName);
                
                for (int j = i; j < textureCount - 1; j++) {
                    texturePool[j] = texturePool[j + 1];
                }
                textureCount--;
            } else {
                printf("Texture %s still in use (refs: %d)\n", textureName, texturePool[i].refCount);
            }
            break;
        }
    }
}

Texture2D GetTextureByName(const char* textureName)
{
    for (int i = 0; i < textureCount; i++) {
        if (strcmp(texturePool[i].name, textureName) == 0) {
            return texturePool[i].texture;
        }
    }
    return (Texture2D){0};
}

GameObject* CreateObject(ObjectType type, const char* name, float x, float y, float z, 
                         bool physics, bool collision)
{
    GameObject** objects = GetObjects();
    int* objectCount = GetObjectCount();
    PlayerPhysicsSettings* settings = GetPlayerSettings();
    
    if (*objectCount >= MAX_OBJECTS)
    {
        printf("Warning: Maximum object limit reached!\n");
        return NULL;
    }
    
    GameObject* obj = (GameObject*)calloc(1, sizeof(GameObject));
    if (!obj) return NULL;
    
    obj->type = type;
    if (name)
    {
        snprintf(obj->name, sizeof(obj->name), "%s", name);
    }
    else
    {
        snprintf(obj->name, sizeof(obj->name), "Object_%d", *objectCount);
    }
    
    obj->position.x = x;
    obj->position.y = y;
    obj->position.z = z;
    
    obj->size.x = 1.0f;
    obj->size.y = 1.0f;
    obj->size.z = 1.0f;

    obj->color = WHITE;
    obj->hasTexture = false;

    obj->hasMaterial = false;
    obj->material.color = WHITE;
    obj->material.shininess = 32.0f;
    obj->material.reflectivity = 0.0f;
    obj->material.useNormalMap = false;
    obj->material.useSpecularMap = false;
    
    switch (type)
    {
        case OBJ_CUBE: 
            obj->color = BLUE;
            obj->material.color = BLUE;
            break;
        case OBJ_SPHERE: 
            obj->color = RED;
            obj->material.color = RED;
            obj->size.x = 2.0f;
            obj->size.y = 2.0f;
            obj->size.z = 2.0f;
            break;
        case OBJ_PLAYER: 
            obj->color = GREEN;
            obj->material.color = GREEN;
            obj->size.x = settings->playerRadius * 2;
            obj->size.y = settings->playerHeight;
            obj->size.z = settings->playerRadius * 2;
            *GetPlayerObject() = obj;
            break;
        case OBJ_PYRAMID: 
            obj->color = YELLOW;
            obj->material.color = YELLOW;
            break;
        case OBJ_CYLINDER: 
            obj->color = ORANGE;
            obj->material.color = ORANGE;
            break;
        case OBJ_PLANE: 
            obj->color = GRAY;
            obj->material.color = GRAY;
            obj->isStatic = true;
            break;
        case OBJ_CONE: 
            obj->color = MAGENTA;
            obj->material.color = MAGENTA;
            break;
        default: 
            obj->color = WHITE;
            obj->material.color = WHITE;
            break;
    }
    
    obj->isVisible = true;
    obj->hasPhysics = physics;
    obj->hasCollision = collision;
    obj->isActive = true;
    obj->isStatic = (type == OBJ_PLANE);
    
    if (physics)
    {
        obj->physics.mass = 1.0f;
        obj->physics.velocity = (Vector3){0, 0, 0};
        obj->physics.acceleration = (Vector3){0, 0, 0};
        obj->physics.isGrounded = false;
        obj->physics.bounceFactor = 0.5f;
        obj->physics.friction = 0.8f;
    }
    
    objects[(*objectCount)++] = obj;
    
    printf("Created object: %s at (%.1f, %.1f, %.1f)\n", obj->name, x, y, z);
    return obj;
}

void DestroyObject(GameObject* obj)
{
    GameObject** objects = GetObjects();
    int* objectCount = GetObjectCount();
    
    for (int i = 0; i < *objectCount; i++)
    {
        if (objects[i] == obj)
        {
            if (obj->hasTexture)
            {
                UnloadTexture(obj->texture);
            }

            if (obj->hasMaterial)
            {
                if (obj->material.diffuseMap.id != 0)
                    UnloadTexture(obj->material.diffuseMap);
                if (obj->material.normalMap.id != 0)
                    UnloadTexture(obj->material.normalMap);
                if (obj->material.specularMap.id != 0)
                    UnloadTexture(obj->material.specularMap);
            }
            
            if (obj == *GetPlayerObject())
            {
                *GetPlayerObject() = NULL;
            }
            
            free(obj);
            
            for (int j = i; j < *objectCount - 1; j++)
            {
                objects[j] = objects[j + 1];
            }
            (*objectCount)--;
            printf("Destroyed object: %s\n", obj->name);
            break;
        }
    }
}

GameObject* FindObject(const char* name)
{
    GameObject** objects = GetObjects();
    int* objectCount = GetObjectCount();
    
    for (int i = 0; i < *objectCount; i++)
    {
        if (objects[i] && strcmp(objects[i]->name, name) == 0)
        {
            return objects[i];
        }
    }
    return NULL;
}

void SetObjectPosition(GameObject* obj, float x, float y, float z)
{
    if (obj)
    {
        obj->position.x = x;
        obj->position.y = y;
        obj->position.z = z;
    }
}

void SetObjectScale(GameObject* obj, float sx, float sy, float sz)
{
    if (obj)
    {
        obj->size.x = sx;
        obj->size.y = sy;
        obj->size.z = sz;
    }
}

void SetObjectRotation(GameObject* obj, float rx, float ry, float rz)
{
    if (obj)
    {
        obj->rotation.x = rx;
        obj->rotation.y = ry;
        obj->rotation.z = rz;
    }
}

void SetObjectTextureOld(GameObject* obj, const char* texturePath)
{
    if (!obj || !texturePath) return;
    
    if (FileExists(texturePath))
    {
        if (obj->hasTexture)
        {
            UnloadTexture(obj->texture);
        }
        obj->texture = LoadTexture(texturePath);
        obj->hasTexture = true;
        printf("Texture set for object: %s (old API)\n", obj->name);
    }
    else
    {
        printf("Warning: Texture file not found: %s\n", texturePath);
    }
}

void SetObjectTexture(GameObject* obj, const char* texturePath, TextureType texType)
{
    if (!obj || !texturePath) return;
    
    if (FileExists(texturePath))
    {
        Texture2D tex = LoadTexture(texturePath);
        
        switch (texType)
        {
            case TEX_DIFFUSE:
                if (obj->material.diffuseMap.id != 0)
                    UnloadTexture(obj->material.diffuseMap);
                obj->material.diffuseMap = tex;
                break;
            case TEX_NORMAL:
                if (obj->material.normalMap.id != 0)
                    UnloadTexture(obj->material.normalMap);
                obj->material.normalMap = tex;
                obj->material.useNormalMap = true;
                break;
            case TEX_SPECULAR:
                if (obj->material.specularMap.id != 0)
                    UnloadTexture(obj->material.specularMap);
                obj->material.specularMap = tex;
                obj->material.useSpecularMap = true;
                break;
        }
        
        obj->hasMaterial = true;
        printf("Material texture set for object: %s (type: %d)\n", obj->name, texType);
    }
    else
    {
        printf("Warning: Texture file not found: %s\n", texturePath);
    }
}

void SetObjectColorOld(GameObject* obj, Color color)
{
    if (obj)
    {
        obj->color = color;
        printf("Color set for object: %s (old API)\n", obj->name);
    }
}

void SetObjectColor(GameObject* obj, Color color)
{
    if (obj)
    {
        obj->material.color = color;
        obj->hasMaterial = true;
        printf("Material color set for object: %s\n", obj->name);
    }
}

void SetObjectMaterial(GameObject* obj, ObjectMaterial material)
{
    if (obj)
    {
        obj->material = material;
        obj->hasMaterial = true;
        printf("Material set for object: %s\n", obj->name);
    }
}

void SetObjectShininess(GameObject* obj, float shininess)
{
    if (obj)
    {
        obj->material.shininess = shininess;
        obj->hasMaterial = true;
        printf("Shininess set for object: %s: %.1f\n", obj->name, shininess);
    }
}

GameObject* CreateCube(const char* name, float x, float y, float z, 
                       bool physics, bool collision, 
                       const char* texturePath, Color color)
{
    GameObject* cube = CreateObject(OBJ_CUBE, name, x, y, z, physics, collision);
    if (cube)
    {
        SetObjectColorOld(cube, color);
        
        if (texturePath && texturePath[0] != '\0')
        {
            SetObjectTextureOld(cube, texturePath);
        }
    }
    return cube;
}

GameObject* CreateSphere(const char* name, float x, float y, float z, 
                         bool physics, bool collision, 
                         Color color, float radius)
{
    GameObject* sphere = CreateObject(OBJ_SPHERE, name, x, y, z, physics, collision);
    if (sphere)
    {
        SetObjectColorOld(sphere, color);
        sphere->size.x = radius * 2;
        sphere->size.y = radius * 2;
        sphere->size.z = radius * 2;
    }
    return sphere;
}

GameObject* CreatePlayer(const char* name, float x, float y, float z, 
                         bool physics, bool collision)
{
    GameObject* player = CreateObject(OBJ_PLAYER, name, x, y, z, physics, collision);
    if (player)
    {
        SetObjectColorOld(player, GREEN);
        PlayerPhysicsSettings* settings = GetPlayerSettings();
        player->size.x = settings->playerRadius * 2;
        player->size.y = settings->playerHeight;
        player->size.z = settings->playerRadius * 2;
        player->physics.friction = settings->groundFriction;
    }
    return player;
}

GameObject* CreatePyramid(const char* name, float x, float y, float z, 
                          bool physics, bool collision, 
                          const char* texturePath, Color color)
{
    GameObject* pyramid = CreateObject(OBJ_PYRAMID, name, x, y, z, physics, collision);
    if (pyramid)
    {
        SetObjectColorOld(pyramid, color);
        
        if (texturePath && texturePath[0] != '\0')
        {
            SetObjectTextureOld(pyramid, texturePath);
        }
    }
    return pyramid;
}

GameObject* CreateCylinder(const char* name, float x, float y, float z, 
                           bool physics, bool collision, 
                           Color color, float radius, float height)
{
    GameObject* cylinder = CreateObject(OBJ_CYLINDER, name, x, y, z, physics, collision);
    if (cylinder)
    {
        SetObjectColorOld(cylinder, color);
        cylinder->size.x = radius * 2;
        cylinder->size.y = height;
        cylinder->size.z = radius * 2;
    }
    return cylinder;
}

GameObject* CreatePlane(const char* name, float x, float y, float z, 
                        float width, float depth, Color color)
{
    GameObject* plane = CreateObject(OBJ_PLANE, name, x, y, z, false, true);
    if (plane)
    {
        SetObjectColorOld(plane, color);
        plane->size.x = width;
        plane->size.z = depth;
        plane->size.y = 0.1f;
        plane->isStatic = true;
    }
    return plane;
}

GameObject* CreateCone(const char* name, float x, float y, float z, 
                       bool physics, bool collision, 
                       Color color, float radius, float height)
{
    GameObject* cone = CreateObject(OBJ_CONE, name, x, y, z, physics, collision);
    if (cone)
    {
        SetObjectColorOld(cone, color);
        cone->size.x = radius * 2;
        cone->size.y = height;
        cone->size.z = radius * 2;
    }
    return cone;
}


GameObject* CreateCubeEx(const char* name, float x, float y, float z, 
                         bool physics, bool collision, 
                         const char* diffusePath, const char* normalPath,
                         const char* specularPath, Color color, float shininess)
{
    GameObject* cube = CreateObject(OBJ_CUBE, name, x, y, z, physics, collision);
    if (cube)
    {
        SetObjectColor(cube, color);
        SetObjectShininess(cube, shininess);
        
        if (diffusePath && diffusePath[0] != '\0')
            SetObjectTexture(cube, diffusePath, TEX_DIFFUSE);
        if (normalPath && normalPath[0] != '\0')
            SetObjectTexture(cube, normalPath, TEX_NORMAL);
        if (specularPath && specularPath[0] != '\0')
            SetObjectTexture(cube, specularPath, TEX_SPECULAR);
    }
    return cube;
}

GameObject* CreateSphereEx(const char* name, float x, float y, float z, 
                           bool physics, bool collision, 
                           Color color, float radius,
                           const char* diffusePath, float shininess)
{
    GameObject* sphere = CreateObject(OBJ_SPHERE, name, x, y, z, physics, collision);
    if (sphere)
    {
        SetObjectColor(sphere, color);
        SetObjectShininess(sphere, shininess);
        sphere->size.x = radius * 2;
        sphere->size.y = radius * 2;
        sphere->size.z = radius * 2;
        
        if (diffusePath && diffusePath[0] != '\0')
            SetObjectTexture(sphere, diffusePath, TEX_DIFFUSE);
    }
    return sphere;
}

GameObject* CreatePyramidEx(const char* name, float x, float y, float z, 
                            bool physics, bool collision, 
                            const char* diffusePath, Color color, float shininess)
{
    GameObject* pyramid = CreateObject(OBJ_PYRAMID, name, x, y, z, physics, collision);
    if (pyramid)
    {
        SetObjectColor(pyramid, color);
        SetObjectShininess(pyramid, shininess);
        
        if (diffusePath && diffusePath[0] != '\0')
            SetObjectTexture(pyramid, diffusePath, TEX_DIFFUSE);
    }
    return pyramid;
}

GameObject* CreateCylinderEx(const char* name, float x, float y, float z, 
                             bool physics, bool collision, 
                             Color color, float radius, float height,
                             const char* diffusePath)
{
    GameObject* cylinder = CreateObject(OBJ_CYLINDER, name, x, y, z, physics, collision);
    if (cylinder)
    {
        SetObjectColor(cylinder, color);
        cylinder->size.x = radius * 2;
        cylinder->size.y = height;
        cylinder->size.z = radius * 2;
        
        if (diffusePath && diffusePath[0] != '\0')
            SetObjectTexture(cylinder, diffusePath, TEX_DIFFUSE);
    }
    return cylinder;
}

GameObject* CreatePlaneEx(const char* name, float x, float y, float z, 
                          float width, float depth, Color color,
                          const char* diffusePath, bool tiled)
{
    GameObject* plane = CreateObject(OBJ_PLANE, name, x, y, z, false, true);
    if (plane)
    {
        SetObjectColor(plane, color);
        plane->size.x = width;
        plane->size.z = depth;
        plane->size.y = 0.1f;
        plane->isStatic = true;
        
        if (diffusePath && diffusePath[0] != '\0')
            SetObjectTexture(plane, diffusePath, TEX_DIFFUSE);
    }
    return plane;
}

GameObject* CreateConeEx(const char* name, float x, float y, float z, 
                         bool physics, bool collision, 
                         Color color, float radius, float height,
                         const char* diffusePath)
{
    GameObject* cone = CreateObject(OBJ_CONE, name, x, y, z, physics, collision);
    if (cone)
    {
        SetObjectColor(cone, color);
        cone->size.x = radius * 2;
        cone->size.y = height;
        cone->size.z = radius * 2;
        
        if (diffusePath && diffusePath[0] != '\0')
            SetObjectTexture(cone, diffusePath, TEX_DIFFUSE);
    }
    return cone;
}

void DrawObject(GameObject* obj)
{
    if (!obj || !obj->isVisible) return;
    
    if (obj->type == OBJ_PLAYER) return;
    
    bool* wireframeMode = GetWireframeMode();
    
    if (*wireframeMode)
    {
        switch (obj->type)
        {
            case OBJ_CUBE:
            case OBJ_PYRAMID:
            case OBJ_PLANE:
                DrawCubeWires(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
                break;
            case OBJ_SPHERE:
                DrawSphereWires(obj->position, obj->size.x / 2, 16, 16, obj->color);
                break;
            case OBJ_CYLINDER:
            case OBJ_CONE:
                DrawCylinderWires(obj->position, obj->size.x / 2, obj->size.x / 2, obj->size.y, 16, obj->color);
                break;
            default:
                DrawCubeWires(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
                break;
        }
    }
    else
    {

        if (obj->hasMaterial && obj->material.diffuseMap.id != 0) {
            switch (obj->type)
            {
                case OBJ_CUBE:
                case OBJ_PLANE:
                    {
                        Model model = LoadModelFromMesh(GenMeshCube(obj->size.x, obj->size.y, obj->size.z));
                        model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = obj->material.diffuseMap;
                        
                        if (obj->material.useNormalMap && obj->material.normalMap.id != 0)
                            model.materials[0].maps[MATERIAL_MAP_NORMAL].texture = obj->material.normalMap;
                        
                        if (obj->material.useSpecularMap && obj->material.specularMap.id != 0)
                            model.materials[0].maps[MATERIAL_MAP_SPECULAR].texture = obj->material.specularMap;

                        DrawModel(model, obj->position, 1.0f, WHITE);
                        UnloadModel(model);
                    }
                    break;
                default:
                    switch (obj->type)
                    {
                        case OBJ_SPHERE:
                            DrawSphere(obj->position, obj->size.x / 2, obj->material.color);
                            break;
                        case OBJ_PYRAMID:
                            DrawCube(obj->position, obj->size.x, obj->size.y * 0.7f, obj->size.z, obj->material.color);
                            break;
                        case OBJ_CYLINDER:
                            DrawCylinder(obj->position, obj->size.x / 2, obj->size.x / 2, 
                                       obj->size.y, 16, obj->material.color);
                            break;
                        case OBJ_CONE:
                            DrawCylinder(obj->position, obj->size.x / 2, 0, obj->size.y, 16, obj->material.color);
                            break;
                        default:
                            DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->material.color);
                            break;
                    }
                    break;
            }
        } else if (obj->hasTexture && obj->texture.id != 0) {
            switch (obj->type)
            {
                case OBJ_CUBE:
                case OBJ_PLANE:
                    {
                        Model cubeModel = LoadModelFromMesh(GenMeshCube(obj->size.x, obj->size.y, obj->size.z));
                        cubeModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = obj->texture;
                        DrawModel(cubeModel, obj->position, 1.0f, WHITE);
                        UnloadModel(cubeModel);
                    }
                    break;
                default:
                    switch (obj->type)
                    {
                        case OBJ_SPHERE:
                            DrawSphere(obj->position, obj->size.x / 2, obj->color);
                            break;
                        case OBJ_PYRAMID:
                            DrawCube(obj->position, obj->size.x, obj->size.y * 0.7f, obj->size.z, obj->color);
                            break;
                        case OBJ_CYLINDER:
                            DrawCylinder(obj->position, obj->size.x / 2, obj->size.x / 2, 
                                       obj->size.y, 16, obj->color);
                            break;
                        case OBJ_CONE:
                            DrawCylinder(obj->position, obj->size.x / 2, 0, obj->size.y, 16, obj->color);
                            break;
                        default:
                            DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, obj->color);
                            break;
                    }
                    break;
            }
        } else {
            Color drawColor = obj->hasMaterial ? obj->material.color : obj->color;
            
            switch (obj->type)
            {
                case OBJ_CUBE:
                    DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, drawColor);
                    break;
                case OBJ_SPHERE:
                    DrawSphere(obj->position, obj->size.x / 2, drawColor);
                    break;
                case OBJ_PYRAMID:
                    DrawCube(obj->position, obj->size.x, obj->size.y * 0.7f, obj->size.z, drawColor);
                    break;
                case OBJ_CYLINDER:
                    DrawCylinder(obj->position, obj->size.x / 2, obj->size.x / 2, 
                               obj->size.y, 16, drawColor);
                    break;
                case OBJ_PLANE:
                    DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, drawColor);
                    break;
                case OBJ_CONE:
                    DrawCylinder(obj->position, obj->size.x / 2, 0, obj->size.y, 16, drawColor);
                    break;
                default:
                    DrawCube(obj->position, obj->size.x, obj->size.y, obj->size.z, drawColor);
                    break;
            }
        }
        
        if (obj->hasCollision)
        {
            DrawCubeWires(obj->position, obj->size.x, obj->size.y, obj->size.z, BLACK);
        }
    }
}