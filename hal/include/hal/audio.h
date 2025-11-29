#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

// Initialize audio system
bool audio_init(void);

// Cleanup audio system
void audio_cleanup(void);

// Music functions
bool audio_load_music(const char* filename);
void audio_play_music(void);
void audio_stop_music(void);
void audio_pause_music(void);
void audio_resume_music(void);
void audio_set_music_volume(int volume); // 0-128

// Sound effect functions
bool audio_load_sound(const char* filename, int sound_id);
void audio_play_sound(int sound_id);
void audio_set_sound_volume(int sound_id, int volume); // 0-128

#endif // AUDIO_H