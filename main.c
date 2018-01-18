#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>

#include "graphics.c"
#include "grid.c"

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
    grid grid = grid_init(width, height);

    // Add glider
    grid.data[2][1] = 1;
    grid.data[3][2] = 1;
    grid.data[1][3] = 1;
    grid.data[2][3] = 1;
    grid.data[3][3] = 1;

    // Add blinker
    grid.data[630][17] = 1;
    grid.data[631][17] = 1;
    grid.data[632][17] = 1;

    // Add R-pentomino
    grid.data[301][300] = 1;
    grid.data[302][300] = 1;
    grid.data[300][301] = 1;
    grid.data[301][301] = 1;
    grid.data[301][302] = 1;


    while (1) {
        grid = grid_step(grid);

        update_texture(renderer, texture, grid);
        
        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            break;
        }
    }

    sdl_quit(renderer, texture, window);

    return 0;

}