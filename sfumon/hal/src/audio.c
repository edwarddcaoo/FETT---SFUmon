#include "audio.h"
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

#define MAX_SOUNDS 32

static Mix_Music* current_music = NULL;
static Mix_Chunk* sounds[MAX_SOUNDS] = {NULL};

bool audio_init(void) {
    printf("Audio: Initializing SDL_mixer\n");
    
    // Initialize SDL audio if not already initialized
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            fprintf(stderr, "Audio: Failed to init SDL audio: %s\n", SDL_GetError());
            return false;
        }
    }
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Audio: Failed to init SDL_mixer: %s\n", Mix_GetError());
        return false;
    }
    
    // Allocate mixing channels
    Mix_AllocateChannels(16);
    
    printf("Audio: Successfully initialized\n");
    return true;
}

void audio_cleanup(void) {
    printf("Audio: Cleanup\n");
    
    // Stop and free music
    if (current_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(current_music);
        current_music = NULL;
    }
    
    // Free all sounds
    for (int i = 0; i < MAX_SOUNDS; i++) {
        if (sounds[i]) {
            Mix_FreeChunk(sounds[i]);
            sounds[i] = NULL;
        }
    }
    
    Mix_CloseAudio();
    printf("Audio: Cleanup complete\n");
}

bool audio_load_music(const char* filename) {
    printf("Audio: Loading music '%s'\n", filename);
    
    // Free previous music if any
    if (current_music) {
        Mix_HaltMusic();
        Mix_FreeMusic(current_music);
        current_music = NULL;
    }
    
    current_music = Mix_LoadMUS(filename);
    if (!current_music) {
        fprintf(stderr, "Audio: Failed to load music '%s': %s\n", filename, Mix_GetError());
        return false;
    }
    
    printf("Audio: Music loaded successfully\n");
    return true;
}

void audio_play_music(void) {
    if (!current_music) {
        fprintf(stderr, "Audio: No music loaded\n");
        return;
    }
    
    if (Mix_PlayMusic(current_music, -1) < 0) {
        fprintf(stderr, "Audio: Failed to play music: %s\n", Mix_GetError());
    } else {
        printf("Audio: Playing music (looping)\n");
    }
}

void audio_stop_music(void) {
    Mix_HaltMusic();
    printf("Audio: Music stopped\n");
}

void audio_pause_music(void) {
    Mix_PauseMusic();
    printf("Audio: Music paused\n");
}

void audio_resume_music(void) {
    Mix_ResumeMusic();
    printf("Audio: Music resumed\n");
}

void audio_set_music_volume(int volume) {
    // Volume range: 0-128
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    Mix_VolumeMusic(volume);
}

bool audio_load_sound(const char* filename, int sound_id) {
    if (sound_id < 0 || sound_id >= MAX_SOUNDS) {
        fprintf(stderr, "Audio: Invalid sound ID %d\n", sound_id);
        return false;
    }
    
    printf("Audio: Loading sound '%s' with id %d\n", filename, sound_id);
    
    // Free previous sound if any
    if (sounds[sound_id]) {
        Mix_FreeChunk(sounds[sound_id]);
        sounds[sound_id] = NULL;
    }
    
    sounds[sound_id] = Mix_LoadWAV(filename);
    if (!sounds[sound_id]) {
        fprintf(stderr, "Audio: Failed to load sound '%s': %s\n", filename, Mix_GetError());
        return false;
    }
    
    printf("Audio: Sound loaded successfully\n");
    return true;
}

void audio_play_sound(int sound_id) {
    if (sound_id < 0 || sound_id >= MAX_SOUNDS) {
        fprintf(stderr, "Audio: Invalid sound ID %d\n", sound_id);
        return;
    }
    
    if (!sounds[sound_id]) {
        fprintf(stderr, "Audio: Sound %d not loaded\n", sound_id);
        return;
    }
    
    // Play on first available channel
    if (Mix_PlayChannel(-1, sounds[sound_id], 0) < 0) {
        fprintf(stderr, "Audio: Failed to play sound: %s\n", Mix_GetError());
    }
}

void audio_set_sound_volume(int sound_id, int volume) {
    if (sound_id < 0 || sound_id >= MAX_SOUNDS) {
        return;
    }
    
    if (sounds[sound_id]) {
        // Volume range: 0-128
        if (volume < 0) volume = 0;
        if (volume > 128) volume = 128;
        Mix_VolumeChunk(sounds[sound_id], volume);
    }
}