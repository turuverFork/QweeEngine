//==================================================================
//QWEE Engine - Lightweight 3D Game Engine
//Copyright (C) 2026 QWEE Development Team
//
//Audio System Implementation
//==================================================================

#include "audio.h"
#include "engine.h"
#include <string.h>
#include <stdlib.h>
#include <math.h> 

#define MAX_AUDIO_FILES 100

static AudioFile* audioFiles[MAX_AUDIO_FILES];
static int audioCount = 0;
static AudioSettings audioSettings = {
    .enabled = true,
    .masterVolume = 1.0f,
    .musicVolume = 0.8f,
    .sfxVolume = 1.0f,
    .mute = false
};

typedef struct {
    AudioFile* audio;
    float remainingTime;
    bool active;
} AudioTimer;

static AudioTimer audioTimers[MAX_AUDIO_FILES];
static int timerCount = 0;

void InitAudioSystem()
{
    InitAudioDevice();
    
    for (int i = 0; i < MAX_AUDIO_FILES; i++) {
        audioFiles[i] = NULL;
        audioTimers[i].active = false;
    }
    
    audioCount = 0;
    timerCount = 0;
    
    printf("Audio system initialized\n");
}

void CloseAudioSystem()
{
    for (int i = 0; i < audioCount; i++) {
        if (audioFiles[i]) {
            if (audioFiles[i]->type == SOUND_EFFECT && IsSoundPlaying(audioFiles[i]->audio.sound)) {
                StopSound(audioFiles[i]->audio.sound);
            } else if (audioFiles[i]->type == MUSIC_TRACK && IsMusicStreamPlaying(audioFiles[i]->audio.music)) {
                StopMusicStream(audioFiles[i]->audio.music);
            }
        }
    }

    for (int i = 0; i < audioCount; i++) {
        if (audioFiles[i]) {
            UnloadAudio(audioFiles[i]);
        }
    }
    
    audioCount = 0;
    CloseAudioDevice();
    printf("Audio system closed\n");
}

void UpdateAudio()
{
    for (int i = 0; i < audioCount; i++) {
        if (audioFiles[i] && audioFiles[i]->loaded) {
            if (audioFiles[i]->type == MUSIC_TRACK) {
                UpdateMusicStream(audioFiles[i]->audio.music);
            }
        }
    }

    for (int i = 0; i < timerCount; i++) {
        if (audioTimers[i].active) {
            audioTimers[i].remainingTime -= GetFrameTime();
            if (audioTimers[i].remainingTime <= 0) {
                StopAudio(audioTimers[i].audio);
                audioTimers[i].active = false;
            }
        }
    }
}

AudioFile* LoadAudio(const char* name, const char* filePath, AudioType type, bool loop)
{
    if (audioCount >= MAX_AUDIO_FILES) {
        printf("Warning: Maximum audio file limit reached!\n");
        return NULL;
    }

    for (int i = 0; i < audioCount; i++) {
        if (audioFiles[i] && strcmp(audioFiles[i]->name, name) == 0) {
            printf("Audio file '%s' already loaded\n", name);
            return audioFiles[i];
        }
    }
    
    AudioFile* audio = (AudioFile*)calloc(1, sizeof(AudioFile));
    if (!audio) return NULL;
    
    snprintf(audio->name, sizeof(audio->name), "%s", name);
    audio->type = type;
    audio->loop = loop;
    audio->volume = 1.0f;
    audio->pitch = 1.0f;

    if (FileExists(filePath)) {
        if (type == SOUND_EFFECT) {
            audio->audio.sound = LoadSound(filePath);

            #ifdef GetSoundTimeLength
            audio->duration = GetSoundTimeLength(audio->audio.sound);
            #else
            audio->duration = 2.0f;
            #endif
            audio->loaded = true;
            printf("Loaded sound: %s (%.2f sec)\n", name, audio->duration);
        } else if (type == MUSIC_TRACK) {
            audio->audio.music = LoadMusicStream(filePath);
            #ifdef GetMusicTimeLength
            audio->duration = GetMusicTimeLength(audio->audio.music);
            #else
            audio->duration = 120.0f; 
            #endif
            audio->loaded = true;
            printf("Loaded music: %s (%.2f sec)\n", name, audio->duration);
        }
    } else {
        printf("Warning: Audio file not found: %s\n", filePath);
        free(audio);
        return NULL;
    }
    
    audioFiles[audioCount++] = audio;
    return audio;
}

void UnloadAudio(AudioFile* audio)
{
    if (!audio) return;
    
    for (int i = 0; i < audioCount; i++) {
        if (audioFiles[i] == audio) {
            if (audio->loaded) {
                if (audio->type == SOUND_EFFECT) {
                    UnloadSound(audio->audio.sound);
                } else if (audio->type == MUSIC_TRACK) {
                    UnloadMusicStream(audio->audio.music);
                }
            }
            free(audio);
            
            for (int j = i; j < audioCount - 1; j++) {
                audioFiles[j] = audioFiles[j + 1];
            }
            audioCount--;
            break;
        }
    }
}

void PlayAudio(AudioFile* audio, float volume)
{
    if (!audio || !audio->loaded || audioSettings.mute) return;
    
    audio->volume = volume * audioSettings.masterVolume;
    
    if (audio->type == SOUND_EFFECT) {
        SetSoundVolume(audio->audio.sound, audio->volume * audioSettings.sfxVolume);
        SetSoundPitch(audio->audio.sound, audio->pitch);
        PlaySound(audio->audio.sound);
    } else if (audio->type == MUSIC_TRACK) {
        SetMusicVolume(audio->audio.music, audio->volume * audioSettings.musicVolume);
        SetMusicPitch(audio->audio.music, audio->pitch);
        PlayMusicStream(audio->audio.music);
    }
    
    printf("Playing audio: %s (volume: %.2f)\n", audio->name, volume);
}

void PlayAudioTimed(AudioFile* audio, float volume, float duration)
{
    if (!audio || !audio->loaded) return;
    
    PlayAudio(audio, volume);

    if (timerCount < MAX_AUDIO_FILES) {
        audioTimers[timerCount].audio = audio;
        audioTimers[timerCount].remainingTime = duration;
        audioTimers[timerCount].active = true;
        timerCount++;
    }
    
    printf("Playing audio timed: %s for %.2f seconds\n", audio->name, duration);
}

void StopAudio(AudioFile* audio)
{
    if (!audio || !audio->loaded) return;
    
    if (audio->type == SOUND_EFFECT) {
        StopSound(audio->audio.sound);
    } else if (audio->type == MUSIC_TRACK) {
        StopMusicStream(audio->audio.music);
    }
    
    printf("Stopped audio: %s\n", audio->name);
}

void PauseAudio(AudioFile* audio)
{
    if (!audio || !audio->loaded) return;
    
    if (audio->type == SOUND_EFFECT) {
        PauseSound(audio->audio.sound);
    } else if (audio->type == MUSIC_TRACK) {
        PauseMusicStream(audio->audio.music);
    }
}

void ResumeAudio(AudioFile* audio)
{
    if (!audio || !audio->loaded) return;
    
    if (audio->type == SOUND_EFFECT) {
        ResumeSound(audio->audio.sound);
    } else if (audio->type == MUSIC_TRACK) {
        ResumeMusicStream(audio->audio.music);
    }
}

void SetAudioVolume(AudioFile* audio, float volume)
{
    if (!audio || !audio->loaded) return;
    
    audio->volume = volume;
    
    if (audio->type == SOUND_EFFECT) {
        SetSoundVolume(audio->audio.sound, volume * audioSettings.sfxVolume * audioSettings.masterVolume);
    } else if (audio->type == MUSIC_TRACK) {
        SetMusicVolume(audio->audio.music, volume * audioSettings.musicVolume * audioSettings.masterVolume);
    }
}

void SetAudioPitch(AudioFile* audio, float pitch)
{
    if (!audio || !audio->loaded) return;
    
    audio->pitch = pitch;
    
    if (audio->type == SOUND_EFFECT) {
        SetSoundPitch(audio->audio.sound, pitch);
    } else if (audio->type == MUSIC_TRACK) {
        SetMusicPitch(audio->audio.music, pitch);
    }
}

void SetEngineMasterVolume(float volume)
{
    audioSettings.masterVolume = volume;
    printf("Master volume set to: %.2f\n", volume);
}

void SetEngineMusicVolume(float volume)
{
    audioSettings.musicVolume = volume;
    printf("Music volume set to: %.2f\n", volume);
}

void SetEngineSFXVolume(float volume)
{
    audioSettings.sfxVolume = volume;
    printf("SFX volume set to: %.2f\n", volume);
}

void SetAudioMuted(bool muted)
{
    audioSettings.mute = muted;
    if (muted) {
        SetEngineMasterVolume(0);
    } else {
        SetEngineMasterVolume(1.0f);
    }
    printf("Audio %s\n", muted ? "muted" : "unmuted");
}

AudioFile* FindAudio(const char* name)
{
    for (int i = 0; i < audioCount; i++) {
        if (audioFiles[i] && strcmp(audioFiles[i]->name, name) == 0) {
            return audioFiles[i];
        }
    }
    return NULL;
}

void PlaySoundAtPosition(const char* name, Vector3 position, float maxDistance)
{
    AudioFile* audio = FindAudio(name);
    if (!audio) return;
    
    Camera3D* cam = GetCamera();
    if (!cam) return;

    float dx = position.x - cam->position.x;
    float dy = position.y - cam->position.y;
    float dz = position.z - cam->position.z;
    float distance = sqrtf(dx*dx + dy*dy + dz*dz);
    
    if (distance > maxDistance) return;
    
    float volume = 1.0f - (distance / maxDistance);
    if (volume < 0.1f) volume = 0.1f;
    
    PlayAudio(audio, volume);
}

void PlaySoundAtObject(const char* name, GameObject* obj, float maxDistance)
{
    if (!obj) return;

    Vector3 position = obj->position;
    PlaySoundAtPosition(name, position, maxDistance);
}