//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Particle System Implementation
//==================================================================

#include "particles.h"
#include "engine.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static ParticleEmitter* emitters[MAX_EMITTERS];
static int emitterCount = 0;

void InitParticleSystem()
{
    for (int i = 0; i < MAX_EMITTERS; i++)
    {
        emitters[i] = NULL;
    }
    emitterCount = 0;
    
    printf("Particle system initialized\n");
}

void CloseParticleSystem()
{
    for (int i = 0; i < emitterCount; i++)
    {
        if (emitters[i])
        {
            DestroyParticleEmitter(emitters[i]);
        }
    }
    emitterCount = 0;
}

void UpdateParticles(float deltaTime)
{
    for (int i = 0; i < emitterCount; i++)
    {
        ParticleEmitter* emitter = emitters[i];
        if (!emitter || !emitter->active) continue;

        if (emitter->loop && emitter->particlesPerSecond > 0)
        {
            emitter->emissionTimer += deltaTime;
            float emitInterval = 1.0f / emitter->particlesPerSecond;
            
            while (emitter->emissionTimer >= emitInterval && 
                   emitter->particleCount < emitter->maxParticles)
            {
                EmitParticles(emitter, 1);
                emitter->emissionTimer -= emitInterval;
            }
        }

        for (int j = 0; j < emitter->maxParticles; j++)
        {
            Particle* p = &emitter->particles[j];
            if (!p->active) continue;
            
            p->lifetime -= deltaTime;
            if (p->lifetime <= 0)
            {
                p->active = false;
                emitter->particleCount--;
                continue;
            }

            p->velocity.x += p->acceleration.x * deltaTime;
            p->velocity.y += p->acceleration.y * deltaTime;
            p->velocity.z += p->acceleration.z * deltaTime;
            
            p->position.x += p->velocity.x * deltaTime;
            p->position.y += p->velocity.y * deltaTime;
            p->position.z += p->velocity.z * deltaTime;
            
            p->acceleration.x += emitter->gravity.x * deltaTime;
            p->acceleration.y += emitter->gravity.y * deltaTime;
            p->acceleration.z += emitter->gravity.z * deltaTime;
            
            p->rotation += p->rotationSpeed * deltaTime;

            float lifeRatio = 1.0f - (p->lifetime / p->maxLifetime);
            
            p->size = p->startSize + (p->endSize - p->startSize) * lifeRatio;
            
            p->color.r = (unsigned char)(p->startColor.r + (p->endColor.r - p->startColor.r) * lifeRatio);
            p->color.g = (unsigned char)(p->startColor.g + (p->endColor.g - p->startColor.g) * lifeRatio);
            p->color.b = (unsigned char)(p->startColor.b + (p->endColor.b - p->startColor.b) * lifeRatio);
            p->color.a = (unsigned char)(p->startColor.a + (p->endColor.a - p->startColor.a) * lifeRatio);
        }
    }
}

void RenderParticles()
{
    Camera3D* cam = GetCamera();
    
    for (int i = 0; i < emitterCount; i++)
    {
        ParticleEmitter* emitter = emitters[i];
        if (!emitter) continue;
        
        for (int j = 0; j < emitter->maxParticles; j++)
        {
            Particle* p = &emitter->particles[j];
            if (!p->active) continue;

            
            Vector3 pos = p->position;
            if (!emitter->worldSpace)
            {
                pos.x += emitter->position.x;
                pos.y += emitter->position.y;
                pos.z += emitter->position.z;
            }

            DrawBillboard(*cam, 
                         (Texture2D){.id = 0, .width = 1, .height = 1, .mipmaps = 1, .format = 0},
                         pos, p->size, p->color);
        }
    }
}

ParticleEmitter* CreateParticleEmitter(const char* name, Vector3 position, ParticleType type)
{
    if (emitterCount >= MAX_EMITTERS)
    {
        printf("Warning: Maximum emitter limit reached!\n");
        return NULL;
    }
    
    ParticleEmitter* emitter = (ParticleEmitter*)calloc(1, sizeof(ParticleEmitter));
    if (!emitter) return NULL;
    
    snprintf(emitter->name, sizeof(emitter->name), "%s", name);
    emitter->position = position;
    emitter->type = type;
    emitter->maxParticles = 100;
    emitter->particlesPerSecond = 10;
    emitter->particleLifetime = 2.0f;
    emitter->minSize = 0.1f;
    emitter->maxSize = 0.3f;
    emitter->startColor = WHITE;
    emitter->endColor = (Color){255, 255, 255, 0};
    emitter->velocityMin = (Vector3){-1, 1, -1};
    emitter->velocityMax = (Vector3){1, 3, 1};
    emitter->gravity = (Vector3){0, -2, 0};
    emitter->loop = true;
    emitter->active = true;
    emitter->worldSpace = true;
    emitter->particleCount = 0;
    emitter->emissionTimer = 0;
    emitter->particles = (Particle*)calloc(emitter->maxParticles, sizeof(Particle));
    if (!emitter->particles)
    {
        free(emitter);
        return NULL;
    }
    
    emitters[emitterCount++] = emitter;
    printf("Created particle emitter: %s\n", name);
    
    return emitter;
}

void DestroyParticleEmitter(ParticleEmitter* emitter)
{
    if (!emitter) return;
    
    for (int i = 0; i < emitterCount; i++)
    {
        if (emitters[i] == emitter)
        {
            if (emitter->particles)
            {
                free(emitter->particles);
            }
            free(emitter);
            
            for (int j = i; j < emitterCount - 1; j++)
            {
                emitters[j] = emitters[j + 1];
            }
            emitterCount--;
            printf("Destroyed particle emitter\n");
            break;
        }
    }
}

ParticleEmitter* FindParticleEmitter(const char* name)
{
    for (int i = 0; i < emitterCount; i++)
    {
        if (emitters[i] && strcmp(emitters[i]->name, name) == 0)
        {
            return emitters[i];
        }
    }
    return NULL;
}

void SetEmitterActive(ParticleEmitter* emitter, bool active)
{
    if (emitter) emitter->active = active;
}

void SetEmitterPosition(ParticleEmitter* emitter, Vector3 position)
{
    if (emitter) emitter->position = position;
}

void EmitParticles(ParticleEmitter* emitter, int count)
{
    if (!emitter || !emitter->active) return;
    
    for (int i = 0; i < count && emitter->particleCount < emitter->maxParticles; i++)
    {
        Particle* p = NULL;
        for (int j = 0; j < emitter->maxParticles; j++)
        {
            if (!emitter->particles[j].active)
            {
                p = &emitter->particles[j];
                break;
            }
        }
        
        if (!p) continue;

        p->position = emitter->position;
        if (!emitter->worldSpace)
        {
            p->position = (Vector3){0, 0, 0};
        }
        
        p->velocity.x = RandomFloat(emitter->velocityMin.x, emitter->velocityMax.x);
        p->velocity.y = RandomFloat(emitter->velocityMin.y, emitter->velocityMax.y);
        p->velocity.z = RandomFloat(emitter->velocityMin.z, emitter->velocityMax.z);
        
        p->acceleration = (Vector3){0, 0, 0};
        p->startColor = emitter->startColor;
        p->endColor = emitter->endColor;
        p->color = emitter->startColor;
        
        p->startSize = RandomFloat(emitter->minSize, emitter->maxSize);
        p->endSize = p->startSize * 0.1f;
        p->size = p->startSize;
        
        p->maxLifetime = emitter->particleLifetime * RandomFloat(0.8f, 1.2f);
        p->lifetime = p->maxLifetime;
        
        p->rotation = RandomFloat(0, 360);
        p->rotationSpeed = RandomFloat(-180, 180);
        p->type = emitter->type;
        p->active = true;
        
        emitter->particleCount++;
    }
}

ParticleEmitter* CreateFireEmitter(Vector3 position, float intensity)
{
    ParticleEmitter* emitter = CreateParticleEmitter("Fire", position, PARTICLE_FIRE);
    if (emitter)
    {
        emitter->maxParticles = (int)(100 * intensity);
        emitter->particlesPerSecond = (int)(50 * intensity);
        emitter->minSize = 0.2f * intensity;
        emitter->maxSize = 0.5f * intensity;
        emitter->startColor = (Color){255, 100, 50, 255};
        emitter->endColor = (Color){255, 50, 0, 0};
        emitter->velocityMin = (Vector3){-0.5f, 0.5f, -0.5f};
        emitter->velocityMax = (Vector3){0.5f, 3.0f * intensity, 0.5f};
        emitter->gravity = (Vector3){0, 0.5f, 0};
        emitter->particleLifetime = 1.5f;
        
        EmitParticles(emitter, emitter->maxParticles / 2);
    }
    return emitter;
}

ParticleEmitter* CreateSmokeEmitter(Vector3 position, float intensity)
{
    ParticleEmitter* emitter = CreateParticleEmitter("Smoke", position, PARTICLE_SMOKE);
    if (emitter)
    {
        emitter->maxParticles = (int)(80 * intensity);
        emitter->particlesPerSecond = (int)(20 * intensity);
        emitter->minSize = 0.3f * intensity;
        emitter->maxSize = 1.0f * intensity;
        emitter->startColor = (Color){100, 100, 100, 200};
        emitter->endColor = (Color){50, 50, 50, 0};
        emitter->velocityMin = (Vector3){-1.0f, 0.2f, -1.0f};
        emitter->velocityMax = (Vector3){1.0f, 1.5f, 1.0f};
        emitter->gravity = (Vector3){0, 0.2f, 0};
        emitter->particleLifetime = 4.0f;
    }
    return emitter;
}

ParticleEmitter* CreateSparkEmitter(Vector3 position, int count)
{
    ParticleEmitter* emitter = CreateParticleEmitter("Sparks", position, PARTICLE_SPARK);
    if (emitter)
    {
        emitter->maxParticles = count * 2;
        emitter->particlesPerSecond = 0; // One-time burst
        emitter->minSize = 0.05f;
        emitter->maxSize = 0.15f;
        emitter->startColor = YELLOW;
        emitter->endColor = (Color){255, 150, 0, 0};
        emitter->velocityMin = (Vector3){-5, 1, -5};
        emitter->velocityMax = (Vector3){5, 8, 5};
        emitter->gravity = (Vector3){0, -10, 0};
        emitter->particleLifetime = 1.0f;
        emitter->loop = false;
        
        EmitParticles(emitter, count);
    }
    return emitter;
}

ParticleEmitter* CreateRainEmitter(Vector3 areaCenter, Vector3 areaSize)
{
    ParticleEmitter* emitter = CreateParticleEmitter("Rain", areaCenter, PARTICLE_RAIN);
    if (emitter)
    {
        emitter->maxParticles = 500;
        emitter->particlesPerSecond = 200;
        emitter->minSize = 0.05f;
        emitter->maxSize = 0.1f;
        emitter->startColor = (Color){200, 200, 255, 200};
        emitter->endColor = (Color){200, 200, 255, 200};
        emitter->velocityMin = (Vector3){-areaSize.x/2, -20, -areaSize.z/2};
        emitter->velocityMax = (Vector3){areaSize.x/2, -25, areaSize.z/2};
        emitter->gravity = (Vector3){0, 0, 0};
        emitter->particleLifetime = areaSize.y / 20.0f;
        emitter->worldSpace = false;
        
        // Initial burst
        EmitParticles(emitter, emitter->maxParticles);
    }
    return emitter;
}

ParticleEmitter* CreateSnowEmitter(Vector3 areaCenter, Vector3 areaSize)
{
    ParticleEmitter* emitter = CreateParticleEmitter("Snow", areaCenter, PARTICLE_SNOW);
    if (emitter)
    {
        emitter->maxParticles = 300;
        emitter->particlesPerSecond = 50;
        emitter->minSize = 0.1f;
        emitter->maxSize = 0.3f;
        emitter->startColor = WHITE;
        emitter->endColor = WHITE;
        emitter->velocityMin = (Vector3){-areaSize.x/2, -1, -areaSize.z/2};
        emitter->velocityMax = (Vector3){areaSize.x/2, -3, areaSize.z/2};
        emitter->gravity = (Vector3){0, -0.5f, 0};
        emitter->particleLifetime = areaSize.y / 2.0f;
        emitter->worldSpace = false;
    }
    return emitter;
}

ParticleEmitter* CreateDustEmitter(Vector3 position, float radius)
{
    ParticleEmitter* emitter = CreateParticleEmitter("Dust", position, PARTICLE_DUST);
    if (emitter)
    {
        emitter->maxParticles = 50;
        emitter->particlesPerSecond = 10;
        emitter->minSize = 0.1f;
        emitter->maxSize = 0.4f;
        emitter->startColor = (Color){200, 180, 150, 150};
        emitter->endColor = (Color){200, 180, 150, 0};
        emitter->velocityMin = (Vector3){-radius, 0.1f, -radius};
        emitter->velocityMax = (Vector3){radius, radius/2, radius};
        emitter->gravity = (Vector3){0, 0.5f, 0};
        emitter->particleLifetime = 2.0f;
    }
    return emitter;
}

void ClearAllParticles()
{
    for (int i = 0; i < emitterCount; i++)
    {
        if (emitters[i])
        {
            DestroyParticleEmitter(emitters[i]);
            i--; 
        }
    }
    emitterCount = 0;
    printf("All particles cleared\n");
}