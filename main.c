#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "grid.c"
#include "main.h"
#include "graphics.c"
#include "pattern.c"

int main(int argc, char** argv)
{
    // Settings
    int screen_width = 1280;
    int screen_height = 720;
    int grid_width = screen_width;
    int grid_height = screen_height;

    // Parse arguments
    char fullscreen = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            fullscreen = 1;
        }
    }

    // Setup data
    AppData data;

    // Graphics
    sdl_init();
    data.window = sdl_create_window(screen_width, screen_height, fullscreen);
    data.renderer = sdl_create_renderer(data.window);
    data.grid_texture = sdl_create_grid_texture(grid_width, grid_height, data.renderer, data.window);
    SDL_Rect debug_rect;
    data.debug_rect = &debug_rect;
    // Grid
    Grid grid = grid_init(grid_width, grid_height);
    data.grid = &grid;
    // Initial values
    data.fps = 0;
    data.ups = 0;
    data.quit = 0;

    // Draw loop, run in separate thread.
    SDL_Thread* draw_thread = SDL_CreateThread(draw_loop, "DrawThread", &data);

    // Update loop, run in separate thread.
    SDL_Thread* update_thread = SDL_CreateThread(update_loop, "UpdateThread", &data);

    // Event loop, run in main thread.
    event_loop(&data);

    // Quit
    SDL_WaitThread(draw_thread, NULL);
    SDL_WaitThread(update_thread, NULL);
    sdl_quit(&data);

    return 0;
}

int draw_loop(void *data) {
    AppData* app_data = (AppData*)(data);

    Uint32 previous_ticks = 0;
    float frametime = 60;
    while (!app_data->quit) {
        // Calculate fps
        Uint32 ticks = SDL_GetTicks();
        Uint32 delta_ticks = ticks - previous_ticks;
        previous_ticks = ticks;

        // Calculate frametime by actual frametime and previous frametime for smoothing
        float smooth_factor = 0.1; 
        frametime = smooth_factor * delta_ticks + (1.0 - smooth_factor) * frametime;
        app_data->fps = 1000.0 / frametime;

        // Update texture
        update_grid_texture(app_data->grid_texture, app_data->grid);

        char debug_text_format[] = "FPS: %.1f UPS: %.1f";
        char debug_text[1000];
        sprintf(debug_text, debug_text_format, app_data->fps, app_data->ups);
        update_debug_texture(debug_text, app_data);

        render(app_data);
    }

    return 0;
}

int event_loop(void *data) {
    AppData* app_data = (AppData*)(data);

    // Event loop
    SDL_Event event;
    while (SDL_WaitEvent(&event) && !app_data->quit) {
        if (event.type == SDL_QUIT) {
            app_data->quit = 1;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                app_data->quit = 1;
            } else if (event.key.keysym.sym == SDLK_SPACE) {
                grid_clear(app_data->grid);
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int width;
            int height;
            SDL_GetWindowSize(app_data->window, &width, &height);

            int x = event.button.x * app_data->grid->width / width;
            int y = event.button.y * app_data->grid->height / height;
            add_pattern(RPENTOMINO, app_data->grid, x, y);
        }
    }

    return 0;
}

int update_loop(void *data) {
    AppData* app_data = (AppData*)(data);

    Uint32 previous_ticks = 0;
    float updatetime = 60;
    while (!app_data->quit) {
        // Generate new generation
        grid_step(app_data->grid);

        // Calculate ups
        Uint32 ticks = SDL_GetTicks();
        Uint32 delta_ticks = ticks - previous_ticks;
        previous_ticks = ticks;

        // Calculate updatetime by actual updatetime and previous updatetime for smoothing
        float smooth_factor = 0.1; 
        updatetime = smooth_factor * delta_ticks + (1.0 - smooth_factor) * updatetime;
        app_data->ups = 1000.0 / updatetime;
    }

    return 0;
}