#ifndef CATCH_H
#define CATCH_H

#include <stdbool.h>
#include <SDL2/SDL.h>
#include "hal/sprite.h"

#define MAX_PETS 20

typedef enum {
    PET_BEAR = 0,
    PET_RACCOON = 1,
    PET_DEER = 2,
    PET_BIGDEER = 3,
    PET_TYPE_COUNT = 4
} PetType;

typedef struct {
    int id;
    PetType type;
    int x;
    int y;
    int room_id;
    bool caught;
    Sprite sprite;
} Pet;

typedef struct {
    Pet pets[MAX_PETS];
    int count;
    int total_caught;
    
    // Respawn configuration - how many of each type should always be available
    int target_counts[PET_TYPE_COUNT];
} PetManager;

// Initialize pet system with target counts for each pet type
void pet_manager_init(PetManager* manager, SDL_Renderer* renderer, 
                      int bear_count, int raccoon_count, int deer_count, int bigdeer_count);

// Spawn initial pets based on target counts
void pet_spawn_initial(PetManager* manager, SDL_Renderer* renderer);

// Check and respawn pets to maintain target counts
void pet_check_respawn(PetManager* manager, SDL_Renderer* renderer);

// Check if player can catch a pet at this position
Pet* pet_check_adjacent(PetManager* manager, int player_x, int player_y, int room_id);

// Check if a pet blocks movement at this position
bool pet_blocks_movement(PetManager* manager, int x, int y, int room_id);

// Catch a pet
void pet_catch(PetManager* manager, Pet* pet);

// Render all pets in current room
void pet_render_all(PetManager* manager, SDL_Renderer* renderer, int current_room_id, int player_x, int player_y);

// Get total caught
int pet_get_total_caught(PetManager* manager);

// Cleanup
void pet_manager_cleanup(PetManager* manager);

#endif // CATCH_H