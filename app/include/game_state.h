#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>
#include "catch.h"

// Save file path
#define SAVE_FILE_PATH "save_data.dat"

// Game state structure to be saved/loaded
typedef struct {
    int animal_catches[PET_TYPE_COUNT];  // Catches for each animal type
    unsigned int checksum;                // Simple validation
} GameState;

/**
 * Initialize the game state system
 * This will attempt to load existing save data
 * Returns true if save data was loaded, false if starting fresh
 */
bool game_state_init(GameState* state);

/**
 * Save the current game state to disk
 * Returns true on success, false on failure
 */
bool game_state_save(const GameState* state);

/**
 * Load game state from disk
 * Returns true if data was loaded successfully, false otherwise
 */
bool game_state_load(GameState* state);

/**
 * Reset game state to zero (also saves the reset state)
 */
void game_state_reset(GameState* state);

/**
 * Update a specific animal catch count and auto-save
 */
void game_state_increment_catch(GameState* state, PetType type);

/**
 * Get the catch count for a specific animal type
 */
int game_state_get_catch_count(const GameState* state, PetType type);

#endif // GAME_STATE_H