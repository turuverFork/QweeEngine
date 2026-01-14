//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Object System Module
//==================================================================

#ifndef OBJECTS_H
#define OBJECTS_H

#include "raylib.h"
#include "physics.h"
#include <stdbool.h>

typedef enum
{
    OBJ_CUBE,
    OBJ_SPHERE,
    OBJ_PLAYER,
    OBJ_PYRAMID,
    OBJ_CYLINDER,
    OBJ_PLANE,
    OBJ_CONE,
    OBJ_CUSTOM
} ObjectType;

typedef enum
{
    TEX_DIFFUSE,
    TEX_NORMAL,
    TEX_SPECULAR
} TextureType;

typedef struct
{
    Color color;
    Texture2D diffuseMap;   
    Texture2D normalMap;      
    Texture2D specularMap;   
    float shininess;         
    float reflectivity;     
    bool useNormalMap;      
    bool useSpecularMap;     
} ObjectMaterial;

struct GameObject
{
    ObjectType type;
    char name[32];
    
    Vector3 position;
    Vector3 size;
    Vector3 rotation;
    
    Color color;
    Texture2D texture;
    bool hasTexture;

    ObjectMaterial material;
    bool hasMaterial;
    
    bool isVisible;
    
    bool hasPhysics;
    PhysicsProperties physics;
    
    bool hasCollision;
    bool isTrigger;
    
    bool isActive;
    bool isStatic;
    
    void* customData;
};

#define MAX_OBJECTS 500
#define MAX_TEXTURES 100

Texture2D LoadGameTexture(const char* path, const char* textureName);
void UnloadGameTexture(const char* textureName);
Texture2D GetTextureByName(const char* textureName);

GameObject* CreateObject(ObjectType type, const char* name, float x, float y, float z, 
                         bool physics, bool collision);
void DestroyObject(GameObject* obj);
GameObject* FindObject(const char* name);
void SetObjectPosition(GameObject* obj, float x, float y, float z);
void SetObjectScale(GameObject* obj, float sx, float sy, float sz);
void SetObjectRotation(GameObject* obj, float rx, float ry, float rz);

void SetObjectMaterial(GameObject* obj, ObjectMaterial material);
void SetObjectTexture(GameObject* obj, const char* texturePath, TextureType texType);
void SetObjectColor(GameObject* obj, Color color);
void SetObjectShininess(GameObject* obj, float shininess);

void SetObjectTextureOld(GameObject* obj, const char* texturePath);  
void SetObjectColorOld(GameObject* obj, Color color);  


GameObject* CreateCubeEx(const char* name, float x, float y, float z, 
                         bool physics, bool collision, 
                         const char* diffusePath, const char* normalPath,
                         const char* specularPath, Color color, float shininess);
GameObject* CreateSphereEx(const char* name, float x, float y, float z, 
                           bool physics, bool collision, 
                           Color color, float radius,
                           const char* diffusePath, float shininess);
GameObject* CreatePlayer(const char* name, float x, float y, float z, 
                         bool physics, bool collision);
GameObject* CreatePyramidEx(const char* name, float x, float y, float z, 
                            bool physics, bool collision, 
                            const char* diffusePath, Color color, float shininess);
GameObject* CreateCylinderEx(const char* name, float x, float y, float z, 
                             bool physics, bool collision, 
                             Color color, float radius, float height,
                             const char* diffusePath);
GameObject* CreatePlaneEx(const char* name, float x, float y, float z, 
                          float width, float depth, Color color,
                          const char* diffusePath, bool tiled);
GameObject* CreateConeEx(const char* name, float x, float y, float z, 
                         bool physics, bool collision, 
                         Color color, float radius, float height,
                         const char* diffusePath);

GameObject* CreateCube(const char* name, float x, float y, float z, 
                       bool physics, bool collision, 
                       const char* texturePath, Color color);
GameObject* CreateSphere(const char* name, float x, float y, float z, 
                         bool physics, bool collision, 
                         Color color, float radius);
GameObject* CreatePyramid(const char* name, float x, float y, float z, 
                          bool physics, bool collision, 
                          const char* texturePath, Color color);
GameObject* CreateCylinder(const char* name, float x, float y, float z, 
                           bool physics, bool collision, 
                           Color color, float radius, float height);
GameObject* CreatePlane(const char* name, float x, float y, float z, 
                        float width, float depth, Color color);
GameObject* CreateCone(const char* name, float x, float y, float z, 
                       bool physics, bool collision, 
                       Color color, float radius, float height);

void DrawObject(GameObject* obj);

#endif