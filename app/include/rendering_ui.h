#ifndef RENDERING_UI_H
#define RENDERING_UI_H

#include <stdbool.h>
#include "catch.h"

// Initialize UI rendering system (loads fonts, etc.)
bool rendering_ui_init(void);

// Draw the HUD with animal counts and reset button
void rendering_ui_draw_hud(PetManager* manager);

// Check if reset button was clicked (call with mouse click coords)
bool rendering_ui_check_reset_click(int mouse_x, int mouse_y);

// Increment the catch count for a specific animal type
void rendering_ui_increment_catch(PetType type);

// Reset all caught animals
void rendering_ui_reset_catches(PetManager* manager);

// Cleanup UI resources
void rendering_ui_cleanup(void);

#endif // RENDERING_UI_H