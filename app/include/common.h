#ifndef COMMON_H
#define COMMON_H

// Grid and window dimensions
#define TILE_SIZE 64
#define GRID_WIDTH 30
#define GRID_HEIGHT 20
#define WINDOW_WIDTH (GRID_WIDTH * TILE_SIZE)
#define WINDOW_HEIGHT (GRID_HEIGHT * TILE_SIZE)

// Game timing
#define TARGET_FPS 30
#define FRAME_DELAY (1000 / TARGET_FPS)

// Movement settings
#define MOVE_DELAY 200
#define ANIMATION_SPEED 8.0f

// Colors (RGB)
#define COLOR_BACKGROUND_R 50
#define COLOR_BACKGROUND_G 150
#define COLOR_BACKGROUND_B 50

#define COLOR_OBSTACLE_R 80
#define COLOR_OBSTACLE_G 80
#define COLOR_OBSTACLE_B 80

#define COLOR_PLAYER_R 255
#define COLOR_PLAYER_G 0
#define COLOR_PLAYER_B 0

#endif