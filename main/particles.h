//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Particle System Module
//==================================================================

#ifndef PARTICLES_H
#define PARTICLES_H

#include "raylib.h"
#include <stdbool.h>

typedef enum
{
    PARTICLE_FIRE,
    PARTICLE_SMOKE,
    PARTICLE_SPARK,
    PARTICLE_RAIN,
    PARTICLE_SNOW,
    PARTICLE_BLOOD,
    PARTICLE_DUST,
    PARTICLE_MAGIC,
    PARTICLE_CUSTOM
} ParticleType;

typedef struct Particle
{
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    Color color;
    Color startColor;
    Color endColor;
    float size;
    float startSize;
    float endSize;
    float lifetime;
    float maxLifetime;
    float rotation;
    float rotationSpeed;
    bool active;
    ParticleType type;
} Particle;

typedef struct ParticleEmitter
{
    char name[32];
    Vector3 position;
    ParticleType type;
    int maxParticles;
    int particlesPerSecond;
    float particleLifetime;
    float minSize;
    float maxSize;
    Color startColor;
    Color endColor;
    Vector3 velocityMin;
    Vector3 velocityMax;
    Vector3 gravity;
    bool loop;
    bool active;
    bool worldSpace;
    
    Particle* particles;
    int particleCount;
    float emissionTimer;
} ParticleEmitter;

#define MAX_EMITTERS 20
#define MAX_PARTICLES_PER_EMITTER 500

void InitParticleSystem();
void CloseParticleSystem();
void UpdateParticles(float deltaTime);
void RenderParticles();

ParticleEmitter* CreateParticleEmitter(const char* name, Vector3 position, ParticleType type);
void DestroyParticleEmitter(ParticleEmitter* emitter);
ParticleEmitter* FindParticleEmitter(const char* name);
void SetEmitterActive(ParticleEmitter* emitter, bool active);
void SetEmitterPosition(ParticleEmitter* emitter, Vector3 position);

ParticleEmitter* CreateFireEmitter(Vector3 position, float intensity);
ParticleEmitter* CreateSmokeEmitter(Vector3 position, float intensity);
ParticleEmitter* CreateSparkEmitter(Vector3 position, int count);
ParticleEmitter* CreateRainEmitter(Vector3 areaCenter, Vector3 areaSize);
ParticleEmitter* CreateSnowEmitter(Vector3 areaCenter, Vector3 areaSize);
ParticleEmitter* CreateDustEmitter(Vector3 position, float radius);

void EmitParticles(ParticleEmitter* emitter, int count);
void ClearAllParticles();

#endif