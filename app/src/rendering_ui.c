#include "rendering_ui.h"
#include "common.h"
#include "hal/display.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

// UI configuration
#define UI_PADDING 20
#define UI_ICON_SIZE 40
#define UI_TEXT_SPACING 50
#define RESET_BUTTON_WIDTH 120
#define RESET_BUTTON_HEIGHT 40

// Font
static TTF_Font* ui_font = NULL;

// Track cumulative catches per animal type
static int total_catches[PET_TYPE_COUNT] = {0};

// Animal type names and emoji/symbols
static const char* ANIMAL_NAMES[] = {
    "Bears",
    "Raccoons", 
    "Deer",
    "Big Deer"
};

// Simple color representations for each animal (for the UI boxes)
static SDL_Color ANIMAL_COLORS[] = {
    {139, 69, 19, 255},   // Brown for bears
    {128, 128, 128, 255}, // Gray for raccoons
    {210, 180, 140, 255}, // Tan for deer
    {160, 82, 45, 255}    // Dark brown for big deer
};

// Reset button rectangle (calculated during draw)
static SDL_Rect reset_button_rect = {0, 0, RESET_BUTTON_WIDTH, RESET_BUTTON_HEIGHT};

bool rendering_ui_init(void) {
    // Initialize catch counts to zero
    for (int i = 0; i < PET_TYPE_COUNT; i++) {
        total_catches[i] = 0;
    }
    
    // lpading fonts
    const char* font_paths[] = {
        "assets/fonts/Arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "C:/Windows/Fonts/arial.ttf"
    };
    
    for (int i = 0; i < 4; i++) {
        ui_font = TTF_OpenFont(font_paths[i], 30);
        if (ui_font) {
            printf("UI: Font loaded from: %s\n", font_paths[i]);
            return true;
        }
    }
    
    fprintf(stderr, "UI: Failed to load any font\n");
    return false;
}

static void draw_animal_count(SDL_Renderer* renderer, const char* name, int count, 
                              SDL_Color color, int y_offset) {
    if (!ui_font) return;
    
    // Draw colored box as icon
    SDL_Rect icon_rect = {
        UI_PADDING,
        UI_PADDING + y_offset,
        UI_ICON_SIZE,
        UI_ICON_SIZE
    };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &icon_rect);
    
    // Draw border around icon
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &icon_rect);
    
    // Create text: "Bears: 3"
    char text[64];
    snprintf(text, sizeof(text), "%s: %d", name, count);
    
    SDL_Color black = {0, 0, 0, 255};
    SDL_Surface* text_surface = TTF_RenderText_Blended(ui_font, text, black);
    
    if (text_surface) {
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        
        if (text_texture) {
            SDL_Rect text_rect = {
                UI_PADDING + UI_ICON_SIZE + 10,
                UI_PADDING + y_offset + (UI_ICON_SIZE - text_surface->h) / 2,
                text_surface->w,
                text_surface->h
            };
            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
            SDL_DestroyTexture(text_texture);
        }
        
        SDL_FreeSurface(text_surface);
    }
}

static void draw_reset_button(SDL_Renderer* renderer) {
    if (!ui_font) return;
    
    // Position button in top right
    reset_button_rect.x = WINDOW_WIDTH - RESET_BUTTON_WIDTH - UI_PADDING;
    reset_button_rect.y = UI_PADDING;
    
    // Draw button background (red)
    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    SDL_RenderFillRect(renderer, &reset_button_rect);
    
    // Draw button border (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &reset_button_rect);
    
    // Draw button text
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* text_surface = TTF_RenderText_Solid(ui_font, "RESET", white);
    
    if (text_surface) {
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        
        if (text_texture) {
            SDL_Rect text_rect = {
                reset_button_rect.x + (RESET_BUTTON_WIDTH - text_surface->w) / 2,
                reset_button_rect.y + (RESET_BUTTON_HEIGHT - text_surface->h) / 2,
                text_surface->w,
                text_surface->h
            };
            SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
            SDL_DestroyTexture(text_texture);
        }
        
        SDL_FreeSurface(text_surface);
    }
}

void rendering_ui_draw_hud(PetManager* manager) {
    if (!manager) return;
    
    SDL_Renderer* renderer = display_get_renderer();
    if (!renderer) return;
    
    // Draw each animal count in the top left using our tracked totals
    for (int type = 0; type < PET_TYPE_COUNT; type++) {
        draw_animal_count(renderer, ANIMAL_NAMES[type], total_catches[type],
                         ANIMAL_COLORS[type], type * UI_TEXT_SPACING);
    }
    
    // Draw reset button in top right
    draw_reset_button(renderer);
}

void rendering_ui_increment_catch(PetType type) {
    if (type >= 0 && type < PET_TYPE_COUNT) {
        total_catches[type]++;
        printf("UI: Caught %s - Total: %d\n", ANIMAL_NAMES[type], total_catches[type]);
    }
}

bool rendering_ui_check_reset_click(int mouse_x, int mouse_y) {
    // Check if click is within reset button bounds
    return (mouse_x >= reset_button_rect.x && 
            mouse_x <= reset_button_rect.x + reset_button_rect.w &&
            mouse_y >= reset_button_rect.y && 
            mouse_y <= reset_button_rect.y + reset_button_rect.h);
}

void rendering_ui_reset_catches(PetManager* manager) {
    (void)manager; // We don't need to modify the PetManager
    
    printf("UI: Resetting all catches...\n");
    
    // Reset our tracked counts
    for (int i = 0; i < PET_TYPE_COUNT; i++) {
        total_catches[i] = 0;
    }
    
    printf("UI: Reset complete - all counts cleared!\n");
}

void rendering_ui_cleanup(void) {
    if (ui_font) {
        TTF_CloseFont(ui_font);
        ui_font = NULL;
        printf("UI: Cleanup complete\n");
    }
}