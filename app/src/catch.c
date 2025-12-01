#include "catch.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Pet sprite paths
static const char* PET_SPRITE_PATHS[] = {
    "assets/sprites/pets/bear.png",
    "assets/sprites/pets/raccoon.png",
    "assets/sprites/pets/deer.png",
    "assets/sprites/pets/bigdeer.png"
};

// Pet names for logging
static const char* PET_NAMES[] = {
    "Bear",
    "Raccoon",
    "Deer",
    "Big Deer"
};

void pet_manager_init(PetManager* manager, SDL_Renderer* renderer,
                      int bear_count, int raccoon_count, int deer_count, int bigdeer_count) {
    (void)renderer; // Will be used in pet_spawn_initial
    
    manager->count = 0;
    manager->total_caught = 0;
    
    // Set target counts for each pet type
    manager->target_counts[PET_BEAR] = bear_count;
    manager->target_counts[PET_RACCOON] = raccoon_count;
    manager->target_counts[PET_DEER] = deer_count;
    manager->target_counts[PET_BIGDEER] = bigdeer_count;
    
    for (int i = 0; i < MAX_PETS; i++) {
        manager->pets[i].caught = true; // Mark all as caught initially
    }
    
    // Seed random number generator
    srand(time(NULL));
    
    printf("Pet Manager: Initialized (Bear:%d, Raccoon:%d, Deer:%d, BigDeer:%d)\n",
           bear_count, raccoon_count, deer_count, bigdeer_count);
}

void pet_spawn_initial(PetManager* manager, SDL_Renderer* renderer) {
    printf("Pet Manager: Spawning initial pets...\n");
    
    // Spawn each pet type according to target counts
    for (int type = 0; type < PET_TYPE_COUNT; type++) {
        for (int i = 0; i < manager->target_counts[type]; i++) {
            if (manager->count >= MAX_PETS) {
                fprintf(stderr, "Pet Manager: Max pets reached\n");
                return;
            }
            
            Pet* pet = &manager->pets[manager->count];
            pet->type = (PetType)type;
            pet->room_id = rand() % 3; // Random room (0, 1, or 2)
            pet->x = 2 + (rand() % (GRID_WIDTH - 4));
            pet->y = 2 + (rand() % (GRID_HEIGHT - 4));
            pet->id = manager->count;
            pet->caught = false;
            
            // Load sprite
            if (!sprite_load(&pet->sprite, renderer, PET_SPRITE_PATHS[type])) {
                fprintf(stderr, "Pet Manager: Failed to load sprite for %s\n", PET_NAMES[type]);
                continue;
            }
            
            printf("  Spawned %s at (%d, %d) in room %d\n", 
                   PET_NAMES[type], pet->x, pet->y, pet->room_id);
            
            manager->count++;
        }
    }
    
    printf("Pet Manager: Spawned %d pets total\n", manager->count);
}

void pet_check_respawn(PetManager* manager, SDL_Renderer* renderer) {
    // Count how many of each type are currently active (not caught)
    int active_counts[PET_TYPE_COUNT] = {0};
    
    for (int i = 0; i < manager->count; i++) {
        if (!manager->pets[i].caught) {
            active_counts[manager->pets[i].type]++;
        }
    }
    
    // Respawn any missing pets
    for (int type = 0; type < PET_TYPE_COUNT; type++) {
        int needed = manager->target_counts[type] - active_counts[type];
        
        for (int i = 0; i < needed; i++) {
            // Find a caught pet slot to reuse, or add new one
            Pet* pet = NULL;
            
            // Try to reuse a caught pet slot of the same type
            for (int j = 0; j < manager->count; j++) {
                if (manager->pets[j].caught && manager->pets[j].type == (PetType)type) {
                    pet = &manager->pets[j];
                    break;
                }
            }
            
            // If no reusable slot, create new pet
            if (!pet && manager->count < MAX_PETS) {
                pet = &manager->pets[manager->count];
                pet->id = manager->count;
                pet->type = (PetType)type;
                
                // Load sprite for new pet
                if (!sprite_load(&pet->sprite, renderer, PET_SPRITE_PATHS[type])) {
                    fprintf(stderr, "Pet Manager: Failed to load sprite for %s\n", PET_NAMES[type]);
                    continue;
                }
                
                manager->count++;
            }
            
            if (pet) {
                // Respawn at random location
                pet->room_id = rand() % 3;
                pet->x = 2 + (rand() % (GRID_WIDTH - 4));
                pet->y = 2 + (rand() % (GRID_HEIGHT - 4));
                pet->caught = false;
                
                printf("↻ Respawned %s at (%d, %d) in room %d\n", 
                       PET_NAMES[type], pet->x, pet->y, pet->room_id);
            }
        }
    }
}

Pet* pet_check_adjacent(PetManager* manager, int player_x, int player_y, int room_id) {
    for (int i = 0; i < manager->count; i++) {
        Pet* pet = &manager->pets[i];
        
        if (!pet->caught && pet->room_id == room_id) {
            int dx = abs(player_x - pet->x);
            int dy = abs(player_y - pet->y);
            
            // Only adjacent orthogonally (not diagonal, not on top)
            if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1)) {
                return pet;
            }
        }
    }
    return NULL;
}

bool pet_blocks_movement(PetManager* manager, int x, int y, int room_id) {
    for (int i = 0; i < manager->count; i++) {
        Pet* pet = &manager->pets[i];
        
        if (!pet->caught && pet->room_id == room_id && pet->x == x && pet->y == y) {
            return true; // This tile is blocked by a pet
        }
    }
    return false;
}

void pet_catch(PetManager* manager, Pet* pet) {
    if (pet->caught) {
        return;
    }
    
    pet->caught = true;
    manager->total_caught++;
    
    printf("✓ Caught %s! Total: %d/%d\n", 
           PET_NAMES[pet->type], manager->total_caught, manager->count);
}

void pet_render_all(PetManager* manager, SDL_Renderer* renderer, int current_room_id, int player_x, int player_y) {
    for (int i = 0; i < manager->count; i++) {
        Pet* pet = &manager->pets[i];
        
        // Only render pets in current room that haven't been caught
        if (!pet->caught && pet->room_id == current_room_id) {
            int pixel_x = pet->x * TILE_SIZE;
            int pixel_y = pet->y * TILE_SIZE;
            
            // Check if player is adjacent orthogonally
            int dx = abs(player_x - pet->x);
            int dy = abs(player_y - pet->y);
            bool is_adjacent = ((dx == 1 && dy == 0) || (dx == 0 && dy == 1));
            
            // Draw white highlight if adjacent
            if (is_adjacent) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_Rect highlight = {
                    pixel_x - 2,
                    pixel_y - 2,
                    TILE_SIZE + 4,
                    TILE_SIZE + 4
                };
                SDL_RenderFillRect(renderer, &highlight);
            }
            
            // Render pet sprite
            sprite_render(&pet->sprite, renderer, pixel_x, pixel_y);
        }
    }
}

int pet_get_total_caught(PetManager* manager) {
    return manager->total_caught;
}

void pet_manager_cleanup(PetManager* manager) {
    for (int i = 0; i < manager->count; i++) {
        sprite_free(&manager->pets[i].sprite);
    }
    printf("Pet Manager: Cleanup complete\n");
}