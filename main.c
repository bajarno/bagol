#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "graphics.c"
#include "grid.c"
#include "pattern.c"

int main(int argc, char** argv)
{
    int width = 640;
    int height = 480;

    // Setup graphics
    sdl_init();
    SDL_Window* window = sdl_create_window(width*2, height*2);
    SDL_Renderer* renderer = sdl_create_renderer(window);
    SDL_Texture* texture = sdl_create_texture(width, height, renderer, window);
    
    // Setup grid
    Grid grid = grid_init(width, height);

    add_pattern(RPENTOMINO, grid, 300, 300);

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
            printf("MouseUP\n");
            int x = event.motion.x/2;
            int y = event.motion.y/2;
            add_pattern(RPENTOMINO, grid, x, y);
        }

        grid = grid_step(grid);

        update_texture(renderer, texture, grid);
    }

    sdl_quit(renderer, texture, window);

    return 0;
}