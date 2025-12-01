#include "game_state.h"
#include <stdio.h>
#include <string.h>

// Simple checksum calculation for data validation
static unsigned int calculate_checksum(const GameState* state) {
    unsigned int sum = 0;
    for (int i = 0; i < PET_TYPE_COUNT; i++) {
        sum += state->animal_catches[i] * (i + 1);
    }
    return sum ^ 0xDEADBEEF;  // XOR with magic number
}

bool game_state_init(GameState* state) {
    // Try to load existing save data
    if (game_state_load(state)) {
        printf("Game State: Loaded existing save data\n");
        for (int i = 0; i < PET_TYPE_COUNT; i++) {
            printf("  Type %d: %d catches\n", i, state->animal_catches[i]);
        }
        return true;
    }
    
    // No save file or corrupted - start fresh
    printf("Game State: Starting fresh (no save data found)\n");
    for (int i = 0; i < PET_TYPE_COUNT; i++) {
        state->animal_catches[i] = 0;
    }
    state->checksum = calculate_checksum(state);
    
    // Create initial save file
    game_state_save(state);
    
    return false;
}

bool game_state_save(const GameState* state) {
    FILE* file = fopen(SAVE_FILE_PATH, "wb");
    if (!file) {
        fprintf(stderr, "Game State: Failed to open save file for writing\n");
        return false;
    }
    
    // Create a copy with updated checksum
    GameState save_state = *state;
    save_state.checksum = calculate_checksum(&save_state);
    
    // Write the entire struct
    size_t written = fwrite(&save_state, sizeof(GameState), 1, file);
    fclose(file);
    
    if (written != 1) {
        fprintf(stderr, "Game State: Failed to write save data\n");
        return false;
    }
    
    printf("Game State: Saved successfully\n");
    return true;
}

bool game_state_load(GameState* state) {
    FILE* file = fopen(SAVE_FILE_PATH, "rb");
    if (!file) {
        // No save file exists yet - this is normal for first run
        return false;
    }
    
    // Read the entire struct
    size_t read = fread(state, sizeof(GameState), 1, file);
    fclose(file);
    
    if (read != 1) {
        fprintf(stderr, "Game State: Failed to read save data\n");
        return false;
    }
    
    // Validate checksum
    unsigned int expected_checksum = calculate_checksum(state);
    if (state->checksum != expected_checksum) {
        fprintf(stderr, "Game State: Save data corrupted (checksum mismatch)\n");
        return false;
    }
    
    return true;
}

void game_state_reset(GameState* state) {
    printf("Game State: Resetting all catches to 0\n");
    
    for (int i = 0; i < PET_TYPE_COUNT; i++) {
        state->animal_catches[i] = 0;
    }
    
    state->checksum = calculate_checksum(state);
    
    // Save the reset state
    game_state_save(state);
}

void game_state_increment_catch(GameState* state, PetType type) {
    if (type < 0 || type >= PET_TYPE_COUNT) {
        fprintf(stderr, "Game State: Invalid pet type %d\n", type);
        return;
    }
    
    state->animal_catches[type]++;
    state->checksum = calculate_checksum(state);
    
    // Auto-save after each catch
    game_state_save(state);
    
    printf("Game State: Incremented type %d to %d (saved)\n", 
           type, state->animal_catches[type]);
}

int game_state_get_catch_count(const GameState* state, PetType type) {
    if (type < 0 || type >= PET_TYPE_COUNT) {
        return 0;
    }
    return state->animal_catches[type];
}