//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Audio System Module
//==================================================================

#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"
#include <stdbool.h>

typedef struct GameObject GameObject;

typedef enum {
    SOUND_EFFECT,
    MUSIC_TRACK,
    AMBIENT_SOUND
} AudioType;

typedef struct AudioFile {
    char name[64];
    AudioType type;
    union {
        Sound sound;
        Music music;
    } audio;
    bool loaded;
    bool loop;
    float duration;
    float volume;
    float pitch;
} AudioFile;

typedef struct {
    bool enabled;
    float masterVolume;
    float musicVolume;
    float sfxVolume;
    bool mute;
} AudioSettings;

void InitAudioSystem();
void CloseAudioSystem();
void UpdateAudio();

AudioFile* LoadAudio(const char* name, const char* filePath, AudioType type, bool loop);
void UnloadAudio(AudioFile* audio);
void PlayAudio(AudioFile* audio, float volume);
void PlayAudioTimed(AudioFile* audio, float volume, float duration);
void StopAudio(AudioFile* audio);
void PauseAudio(AudioFile* audio);
void ResumeAudio(AudioFile* audio);
void SetAudioVolume(AudioFile* audio, float volume);
void SetAudioPitch(AudioFile* audio, float pitch);

void SetEngineMasterVolume(float volume);
void SetEngineMusicVolume(float volume);
void SetEngineSFXVolume(float volume);
void SetAudioMuted(bool muted);

AudioFile* FindAudio(const char* name);

void PlaySoundAtPosition(const char* name, Vector3 position, float maxDistance);
void PlaySoundAtObject(const char* name, GameObject* obj, float maxDistance);

#endif