#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "graphics.c"
#include "grid.c"
#include "pattern.c"

int main(int argc, char** argv)
{
    // Settings
    int screen_width = 2560;
    int screen_height = 1440;
    int grid_width = screen_width/2;
    int grid_height = screen_height/2;

    // Parse arguments
    char fullscreen = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            fullscreen = 1;
        }
    }

    // Setup graphics
    sdl_init();
    SDL_Window* window = sdl_create_window(screen_width, screen_height, fullscreen);
    SDL_Renderer* renderer = sdl_create_renderer(window);
    SDL_Texture* texture = sdl_create_texture(grid_width, grid_height, renderer, window);
    
    // Setup grid
    Grid grid = grid_init(grid_width, grid_height);

    add_pattern(RPENTOMINO, grid, 300, 300);

    // Main loop
    while (1) {
        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                break;
            } else if (event.key.keysym.sym == SDLK_SPACE) {
                grid_clear(grid);
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.motion.x * grid_width / screen_width;
            int y = event.motion.y * grid_height / screen_height;
            add_pattern(RPENTOMINO, grid, x, y);
        }

        grid = grid_step(grid);

        update_texture(renderer, texture, grid);
    }

    sdl_quit(renderer, texture, window);

    return 0;
}