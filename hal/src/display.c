#include "display.h"
#include <stdio.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

bool display_init(const char *title, int width, int height)
{
    // Try kmsdrm first (for BeagleY-AI LCD), fallback to default for host
    if (SDL_setenv("SDL_VIDEODRIVER", "kmsdrm", 0) == 0)
    {
        // kmsdrm set successfully, also set render driver
        SDL_setenv("SDL_RENDER_DRIVER", "opengles2", 1);
        printf("Attempting to use kmsdrm driver (target device)\n");
    }
    else
    {
        printf("Using default SDL video driver (host machine)\n");
    }

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        // If kmsdrm failed, try again without it
        printf("First init failed: %s\n", SDL_GetError());
        printf("Retrying with default driver...\n");

        SDL_setenv("SDL_VIDEODRIVER", "", 1); // Clear the driver
        SDL_setenv("SDL_RENDER_DRIVER", "", 1);

        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            return false;
        }
    }

    // Get display info
    SDL_DisplayMode current_mode;
    SDL_GetCurrentDisplayMode(0, &current_mode);
    printf("Display mode: %dx%d @ %dHz\n",
           current_mode.w, current_mode.h, current_mode.refresh_rate);

    // For host: use windowed mode. For target: use fullscreen
    Uint32 window_flags = SDL_WINDOW_SHOWN;
    int window_w = width;
    int window_h = height;

    // Check if we're likely on the target (800x480 display = BeagleY LCD)
    if (current_mode.w == 800 && current_mode.h == 480)
    {
        window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
        window_w = current_mode.w;
        window_h = current_mode.h;
        printf("Target device detected - using fullscreen\n");
    }
    else
    {
        printf("Host machine detected - using windowed mode\n");
    }

    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_w,
        window_h,
        window_flags);

    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1,
                                  SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL)
    {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Set logical size to scale game content
    SDL_RenderSetLogicalSize(renderer, width, height);

    // Use linear filtering for smooth scaling
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    printf("SDL2 initialized successfully!\n");
    printf("Logical size: %dx%d, Physical size: %dx%d\n",
           width, height, window_w, window_h);

    return true;
}

void display_cleanup(void)
{
    if (renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window)
    {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    SDL_Quit();
}

SDL_Renderer *display_get_renderer(void)
{
    return renderer;
}

void display_clear(int r, int g, int b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_RenderClear(renderer);
}

void display_present(void)
{
    SDL_RenderPresent(renderer);
}