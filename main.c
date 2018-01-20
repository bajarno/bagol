#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "grid.c"
#include "main.h"
#include "graphics.c"
#include "pattern.c"

int main(int argc, char** argv)
{
    // Settings
    int screen_width = 2560;
    int screen_height = 1440;
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
    data.texture = sdl_create_texture(grid_width, grid_height, data.renderer, data.window);
    // Grid
    Grid grid = grid_init(grid_width, grid_height);
    data.grid = &grid;
    // Initial values
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
    sdl_quit(data);

    return 0;
}

int draw_loop(void *data) {
    AppData* app_data = (AppData*)(data);

    while (!app_data->quit) {
        update_texture(app_data->renderer, app_data->texture, app_data->grid);
        
        SDL_Delay(1/60);
    }

    return 0;
}

int event_loop(void *data) {
    AppData* app_data = (AppData*)(data);

    // Event loop
    SDL_Event event;
    while (SDL_WaitEvent(&event) && !app_data->quit) {
        if (event.type == SDL_KEYDOWN) {
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

    while (!app_data->quit) {
        grid_step(app_data->grid);
    }

    return 0;
}