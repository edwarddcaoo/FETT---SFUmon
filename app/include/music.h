#ifndef MUSIC_H
#define MUSIC_H

#include <stdbool.h>
#include "map.h"

// Initialize music system and preload all tracks
bool music_init(void);

// Start music after delay
bool music_start_delayed(void);  // Add this line

// Update music based on current game state
void music_update(Room* current_room, int player_x, int player_y);

// Change music when switching rooms
void music_change_room(const char* new_music_path);

// Cleanup music system
void music_cleanup(void);

#endif // MUSIC_H