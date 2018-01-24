#import "graphics.h"
#import "grid.c"

TTF_Font* courier_new_font;
SDL_Color white_color = {255, 255, 255}; 

void sdl_init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        fprintf(stderr, "could not initialize SDL: %s\n", SDL_GetError());
    }

    if (TTF_Init() != 0){
        fprintf(stderr, "could not initialize SDL_ttf: %s\n", TTF_GetError());
    }

    // courier_new_font = TTF_OpenFont("/Library/Fonts/Courier New.ttf", 11);
    courier_new_font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 11);


    if (courier_new_font == NULL){
        fprintf(stderr, "could not load font: %s\n", TTF_GetError());
    }
}

SDL_Window* sdl_create_window(int width, int height, int fullscreen) {
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

    if (fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_SetWindowSize(window, 2560, 1440);
    }

    return window;
}

SDL_Renderer* sdl_create_renderer(SDL_Window* window) {
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL){
        SDL_DestroyWindow(window);
        fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return renderer;
}

SDL_Texture* sdl_create_grid_texture(int width, int height, SDL_Renderer* renderer, SDL_Window* window) {
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, width, height);    

    if (texture == NULL){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        fprintf(stderr, "could not create texture: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return texture;
}

void update_grid_texture(SDL_Texture* texture, Grid* grid) {
    int width;
    int height;

    SDL_QueryTexture(texture, NULL, NULL, &width, &height);

    int size = width * height;
    uint8_t* pixels = malloc(size * sizeof(*pixels));

    SDL_AtomicLock(&grid->read_lock);
    for (int i = 0; i < grid->width * grid->height; i++) {
        pixels[i] = (grid->data[i % grid->width][i / grid->width] & STATEMASK) ? 255 : 0;
    }
    SDL_AtomicUnlock(&grid->read_lock);

    SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(uint8_t));

    free(pixels);
}

void update_debug_texture(char* text, AppData * app_data) {
    SDL_DestroyTexture(app_data->debug_texture);

    SDL_Surface* surface = TTF_RenderText_Solid(courier_new_font, text, white_color);
    app_data->debug_texture = SDL_CreateTextureFromSurface(app_data->renderer, surface);
    
    int window_width;
    SDL_GetWindowSize(app_data->window, &window_width, NULL);

    app_data->debug_rect->w = surface->w;
    app_data->debug_rect->h = surface->h;
    app_data->debug_rect->x = window_width - surface->w;
    app_data->debug_rect->y = 0;

    SDL_FreeSurface(surface);
}

void render(AppData * app_data) {
    SDL_RenderClear(app_data->renderer);
    SDL_RenderCopy(app_data->renderer, app_data->grid_texture, NULL, NULL);
    SDL_RenderCopy(app_data->renderer, app_data->debug_texture, NULL, app_data->debug_rect);
    SDL_RenderPresent(app_data->renderer);
}

void sdl_quit(AppData * data) {
    SDL_DestroyTexture(data->grid_texture);
    SDL_DestroyTexture(data->debug_texture);    
    SDL_DestroyRenderer(data->renderer);
    SDL_DestroyWindow(data->window);
    SDL_Quit();
}
