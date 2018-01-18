#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL2/SDL.h>

typedef struct {
    int width;
    int height;
    char ** data;
} grid;

void print_grid(grid grid) {
    for (int x = 0; x < grid.width; x++) {
        for (int y = 0; y < grid.height; y++) {
            printf("%d", grid.data[x][y]);
        }
        printf("\n");
    }
}

grid step_grid(grid grid) {

    char ** new_data = (char **)malloc(sizeof(char *) * grid.width);
    new_data[0] = (char *)malloc(sizeof(char) * grid.height * grid.width);

    for(int i = 0; i < grid.width; i++) {
        new_data[i] = (*new_data + grid.height * i);
    }

    for (int x = 0; x < grid.width; x++) {
        for (int y = 0; y < grid.height; y++) {
            int living = 0;
            
            if (x > 0) {
                living += grid.data[x-1][y];
            }
            if (y > 0) {
                living += grid.data[x][y-1];
            }
            if (x < grid.width - 1) {
                living += grid.data[x+1][y];
            }
            if (y < grid.height - 1) {
                living += grid.data[x][y+1];
            }
            if (x > 0 && y > 0) {
                living += grid.data[x-1][y-1];
            }
            if (x > 0 && y < grid.height -1) {
                living += grid.data[x-1][y+1];
            }
            if (x < grid.width - 1 && y > 0) {
                living += grid.data[x+1][y-1];
            }
            if (x < grid.width -1 && y < grid.height - 1) {
                living += grid.data[x+1][y+1];
            }

            if (living < 2 || living > 3) {
                new_data[x][y] = 0;
            } else if (living == 3) {
                new_data[x][y] = 1;
            } else {
                new_data[x][y] = grid.data[x][y];
            }
        }
    }

    grid.data = new_data;

    return grid;
}

grid init_grid(int width, int height) {
    grid grid;
    grid.width = width;
    grid.height = height;
    grid.data = (char **)malloc(sizeof(char *) * width);
    grid.data[0] = (char *)malloc(sizeof(char) * height * width);

    for(int i = 0; i < width; i++) {
        grid.data[i] = (*grid.data + height * i);
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            grid.data[x][y] = 0;
        }
    }

    return grid;
}

int main(int argc, char** argv)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow(
        "hello_sdl2",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        640, 480,
        SDL_WINDOW_SHOWN
        );
    if (window == NULL){
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == NULL){
        SDL_DestroyWindow(window);
        fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Load bitmap
    SDL_Surface *bitmap = SDL_LoadBMP("hello.bmp");
    if (bitmap == NULL){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        fprintf(stderr, "could not load bitmap: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create texture
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, 640, 480);    
    SDL_FreeSurface(bitmap);
    if (texture == NULL){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        fprintf(stderr, "could not create texture: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    
    int width = 640;
    int height = 480;

    grid grid = init_grid(width, height);

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

    for (int i = 0; i < 100; i++) {
        grid = step_grid(grid);

        char pixels[640*480];
        memset(pixels, 255, 640 * 480 * sizeof(char));
        for (int i = 0; i < 640*480; i++) {
            pixels[i] = grid.data[i%640][i/640]*255;
        }


        SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(char));
    
        //First clear the renderer
        SDL_RenderClear(renderer);
        //Draw the texture
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        //Update the screen
        SDL_RenderPresent(renderer);
        //Take a quick break after all that hard work
        SDL_Delay(10);
    }

    // delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;




    
}