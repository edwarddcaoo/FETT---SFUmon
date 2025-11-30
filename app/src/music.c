#include "music.h"
#include "hal/audio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static char current_music_path[128] = "";
static bool was_near_professor = false;

// Helper function to check if player is near Professor Matthew
static bool is_near_professor_matthew(Room* room, int player_x, int player_y) {
    for (int i = 0; i < room->npc_count; i++) {
        if (strcmp(room->npcs[i].name, "Professor Matthew") == 0 && !room->npcs[i].caught) {
            int dx = abs(player_x - room->npcs[i].x);
            int dy = abs(player_y - room->npcs[i].y);
            int distance = dx + dy; // Manhattan distance
            
            // Within 3 tiles
            if (distance <= 3) {
                return true;
            }
        }
    }
    return false;
}

bool music_init(void) {
    printf("Music: Initialization complete (music will start shortly)\n");
    
    // Just set the path, don't load yet
    strcpy(current_music_path, "assets/music/main_hall.ogg");
    
    return true;
}

// Add this new function
bool music_start_delayed(void) {
    printf("Music: Loading main hall music...\n");
    
    if (!audio_load_music(current_music_path)) {
        fprintf(stderr, "Music: Failed to load music\n");
        return false;
    }
    
    audio_play_music();
    audio_set_music_volume(64);
    printf("Music: Playing!\n");
    
    return true;
}

void music_update(Room* current_room, int player_x, int player_y) {
    // Check if near Professor Matthew for encounter music
    bool near_professor = is_near_professor_matthew(current_room, player_x, player_y);
    
    // Handle music transitions - only when state changes
    if (near_professor && !was_near_professor) {
        // Just entered proximity - switch to encounter music
        audio_stop_music();
        audio_load_music("assets/music/matthew.ogg");
        audio_play_music();
        audio_set_music_volume(64);
        was_near_professor = true;
        printf("🎵 Encounter music started!\n");
    } else if (!near_professor && was_near_professor) {
        // Just left proximity - switch back to room music
        audio_stop_music();
        audio_load_music(current_music_path);
        audio_play_music();
        audio_set_music_volume(64);
        was_near_professor = false;
        printf("🎵 Room music resumed\n");
    }
}

void music_change_room(const char* new_music_path) {
    // Only change if different from current
    if (strcmp(current_music_path, new_music_path) != 0) {
        strcpy(current_music_path, new_music_path);
        audio_stop_music();
        audio_load_music(current_music_path);
        audio_play_music();
        audio_set_music_volume(64);
        was_near_professor = false;  // Reset when changing rooms
        printf("🎵 Changed to room music: %s\n", new_music_path);
    }
}

void music_cleanup(void) {
    audio_stop_music();
    printf("Music: Cleanup complete\n");
}