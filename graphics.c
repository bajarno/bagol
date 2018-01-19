#import "grid.c"

void sdl_init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "could not initialize SDL: %s\n", SDL_GetError());
    }
}

SDL_Window* sdl_create_window(int width, int height) {
    SDL_Window *window = SDL_CreateWindow(
        "hello_sdl2",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN
        );
    if (window == NULL){
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return window;
}

SDL_Renderer* sdl_create_renderer(SDL_Window* window) {
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == NULL){
        SDL_DestroyWindow(window);
        fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return renderer;
}

SDL_Texture* sdl_create_texture(int width, int height, SDL_Renderer* renderer, SDL_Window* window) {
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, width, height);    

    if (texture == NULL){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        fprintf(stderr, "could not create texture: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return texture;
}

void update_texture(SDL_Renderer* renderer, SDL_Texture* texture, Grid grid) {
    int width;
    int height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    char pixels[width * height];
    memset(pixels, 255, width * height * sizeof(char));
    for (int i = 0; i < grid.width * grid.height; i++) {
        pixels[i] = grid.data[i % grid.width][i / grid.width] * 255;
    }

    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(char));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void sdl_quit(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Window* window) {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}