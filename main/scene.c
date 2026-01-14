//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Scene System Implementation
//==================================================================

#include "scene.h"
#include "engine.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static Scene scenes[MAX_SCENES];
static int sceneCount = 0;
static Scene* currentScene = NULL;

void InitSceneSystem()
{
    for (int i = 0; i < MAX_SCENES; i++)
    {
        scenes[i].name[0] = '\0';
        scenes[i].type = SCENE_3D;
        scenes[i].initFunc = NULL;
        scenes[i].updateFunc = NULL;
        scenes[i].renderFunc = NULL;
        scenes[i].closeFunc = NULL;
        scenes[i].isActive = false;
        scenes[i].isLoaded = false;
        scenes[i].sceneObjectCount = 0;
        
        for (int j = 0; j < MAX_SCENE_OBJECTS; j++)
        {
            scenes[i].sceneObjects[j] = NULL;
        }
    }
    
    sceneCount = 0;
    currentScene = NULL;
    
    printf("Scene system initialized\n");
}

void RegisterScene(const char* name, SceneType type, 
                  SceneFunction init, SceneFunction update, 
                  SceneFunction render, SceneFunction close)
{
    if (sceneCount >= MAX_SCENES)
    {
        printf("Warning: Maximum scene limit reached!\n");
        return;
    }
    
    for (int i = 0; i < sceneCount; i++)
    {
        if (strcmp(scenes[i].name, name) == 0)
        {
            printf("Warning: Scene '%s' already registered\n", name);
            return;
        }
    }
    
    Scene* scene = &scenes[sceneCount++];
    snprintf(scene->name, sizeof(scene->name), "%s", name);
    scene->type = type;
    scene->initFunc = init;
    scene->updateFunc = update;
    scene->renderFunc = render;
    scene->closeFunc = close;
    scene->isActive = false;
    scene->isLoaded = false;
    scene->sceneObjectCount = 0;
    
    printf("Registered scene: %s (%s)\n", name, type == SCENE_2D ? "2D" : "3D");
}

bool LoadScene(const char* name)
{
    Scene* scene = NULL;
    
    for (int i = 0; i < sceneCount; i++)
    {
        if (strcmp(scenes[i].name, name) == 0)
        {
            scene = &scenes[i];
            break;
        }
    }
    
    if (!scene)
    {
        printf("Error: Scene '%s' not found\n", name);
        return false;
    }
    
    if (scene->isLoaded)
    {
        printf("Scene '%s' already loaded\n", name);
        return true;
    }

    if (currentScene && currentScene->isLoaded)
    {
        if (currentScene->closeFunc)
        {
            currentScene->closeFunc(currentScene->name, currentScene->type);
        }
        currentScene->isActive = false;
        currentScene->isLoaded = false;
        ClearSceneObjects(currentScene->name);
    }

    printf("Loading scene: %s\n", name);
    scene->isLoaded = true;
    
    if (scene->initFunc)
    {
        scene->initFunc(scene->name, scene->type);
    }
    
    currentScene = scene;
    
    printf("Scene '%s' loaded successfully\n", name);
    return true;
}

void UnloadScene(const char* name)
{
    Scene* scene = NULL;
    
    for (int i = 0; i < sceneCount; i++)
    {
        if (strcmp(scenes[i].name, name) == 0)
        {
            scene = &scenes[i];
            break;
        }
    }
    
    if (!scene || !scene->isLoaded)
    {
        return;
    }
    
    printf("Unloading scene: %s\n", name);
    
    if (scene->closeFunc)
    {
        scene->closeFunc(scene->name, scene->type);
    }
    
    ClearSceneObjects(name);
    scene->isActive = false;
    scene->isLoaded = false;
    
    if (currentScene == scene)
    {
        currentScene = NULL;
    }
    
    printf("Scene '%s' unloaded\n", name);
}

void SwitchScene(const char* name)
{
    if (!LoadScene(name))
    {
        printf("Failed to switch to scene: %s\n", name);
        return;
    }
    
    if (currentScene)
    {
        currentScene->isActive = true;
        printf("Switched to scene: %s\n", name);
    }
}

void UpdateCurrentScene()
{
    if (!currentScene || !currentScene->isActive || !currentScene->isLoaded)
    {
        return;
    }
    
    if (currentScene->updateFunc)
    {
        currentScene->updateFunc(currentScene->name, currentScene->type);
    }
}

void RenderCurrentScene()
{
    if (!currentScene || !currentScene->isActive || !currentScene->isLoaded)
    {
        return;
    }
    
    if (currentScene->renderFunc)
    {
        currentScene->renderFunc(currentScene->name, currentScene->type);
    }
}

Scene* GetCurrentScene()
{
    return currentScene;
}

Scene* FindScene(const char* name)
{
    for (int i = 0; i < sceneCount; i++)
    {
        if (strcmp(scenes[i].name, name) == 0)
        {
            return &scenes[i];
        }
    }
    return NULL;
}

void AddObjectToScene(const char* sceneName, GameObject* obj)
{
    Scene* scene = FindScene(sceneName);
    if (!scene)
    {
        printf("Error: Scene '%s' not found\n", sceneName);
        return;
    }
    
    if (scene->sceneObjectCount >= MAX_SCENE_OBJECTS)
    {
        printf("Warning: Scene '%s' object limit reached!\n", sceneName);
        return;
    }
    
    scene->sceneObjects[scene->sceneObjectCount++] = obj;
    printf("Added object '%s' to scene '%s'\n", obj->name, sceneName);
}

void RemoveObjectFromScene(const char* sceneName, GameObject* obj)
{
    Scene* scene = FindScene(sceneName);
    if (!scene) return;
    
    for (int i = 0; i < scene->sceneObjectCount; i++)
    {
        if (scene->sceneObjects[i] == obj)
        {
            for (int j = i; j < scene->sceneObjectCount - 1; j++)
            {
                scene->sceneObjects[j] = scene->sceneObjects[j + 1];
            }
            scene->sceneObjectCount--;
            printf("Removed object '%s' from scene '%s'\n", obj->name, sceneName);
            break;
        }
    }
}

void ClearSceneObjects(const char* sceneName)
{
    Scene* scene = FindScene(sceneName);
    if (!scene) return;
    
    for (int i = 0; i < scene->sceneObjectCount; i++)
    {
        if (scene->sceneObjects[i])
        {
            DestroyObject(scene->sceneObjects[i]);
        }
    }
    
    scene->sceneObjectCount = 0;
    printf("Cleared all objects from scene '%s'\n", sceneName);
}

bool IsScene2D()
{
    return currentScene && currentScene->type == SCENE_2D;
}

bool IsScene3D()
{
    return currentScene && currentScene->type == SCENE_3D;
}

const char* GetCurrentSceneName()
{
    return currentScene ? currentScene->name : "No Scene";
}

SceneType GetCurrentSceneType()
{
    return currentScene ? currentScene->type : SCENE_3D;
}