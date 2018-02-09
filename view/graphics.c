#include "camera.c"

#include "graphics.h"

#include "data_texture.c"

TTF_Font *courier_new_font;
SDL_Color white_color = {255, 255, 255};

RenderData *sdl_init(int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "could not initialize SDL: %s\n", SDL_GetError());
    }

    if (TTF_Init() != 0)
    {
        fprintf(stderr, "could not initialize SDL_ttf: %s\n", TTF_GetError());
    }

    courier_new_font = TTF_OpenFont("./view/Courier New.ttf", 11);

    if (courier_new_font == NULL)
    {
        fprintf(stderr, "could not load font: %s\n", TTF_GetError());
    }

    RenderData *render_data = malloc(sizeof(RenderData));

    render_data->window = sdl_create_window(width, height);
    render_data->window_width = width;
    render_data->window_height = height;

    render_data->renderer = sdl_create_renderer(render_data->window);
    render_data->data_texture = data_texture_init(render_data->renderer, width, height);

    render_data->debug_mode = 0;

    SDL_Rect debug_rect;
    render_data->debug_rect = &debug_rect;
    render_data->debug_texture = SDL_CreateTexture(render_data->renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, 0, 0);

    uint64_t camera_x = INITIAL_QUAD_POSITION;
    camera_x = camera_x * 6 - width / 2; // Adustments on separate line because of uint32_t to uint64_t conversion.
    uint64_t camera_y = INITIAL_QUAD_POSITION;
    camera_y = camera_y * 6 - height / 2; // Adjustments on separate line because of uint32_t to uint64_t conversion.
    render_data->camera = camera_init(camera_x, camera_y, width, height);

    render_data->write_lock = 0;

    return render_data;
}

SDL_Window *sdl_create_window(int width, int height)
{
    SDL_Window *window = SDL_CreateWindow(
        "bagol",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == NULL)
    {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return window;
}

SDL_Renderer *sdl_create_renderer(SDL_Window *window)
{
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL)
    {
        SDL_DestroyWindow(window);
        fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return renderer;
}

void update_debug_texture(char *text, RenderData *render_data)
{
    SDL_AtomicLock(&render_data->write_lock);

    SDL_DestroyTexture(render_data->debug_texture);

    SDL_Surface *surface = TTF_RenderText_Solid(courier_new_font, text, white_color);
    render_data->debug_texture = SDL_CreateTextureFromSurface(render_data->renderer, surface);

    int window_width;
    SDL_GetWindowSize(render_data->window, &window_width, NULL);

    render_data->debug_rect->w = surface->w;
    render_data->debug_rect->h = surface->h;
    render_data->debug_rect->x = window_width - surface->w;
    render_data->debug_rect->y = 0;

    SDL_FreeSurface(surface);

    SDL_AtomicUnlock(&render_data->write_lock);
}

void render(RenderData *render_data)
{
    SDL_AtomicLock(&render_data->write_lock);

    SDL_RenderClear(render_data->renderer);
    SDL_RenderCopy(render_data->renderer, render_data->data_texture, NULL, NULL);
    SDL_RenderCopy(render_data->renderer, render_data->debug_texture, NULL, render_data->debug_rect);
    SDL_RenderPresent(render_data->renderer);

    SDL_AtomicUnlock(&render_data->write_lock);
}

void sdl_quit(RenderData *render_data)
{
    SDL_DestroyTexture(render_data->data_texture);
    SDL_DestroyTexture(render_data->debug_texture);
    SDL_DestroyRenderer(render_data->renderer);
    SDL_DestroyWindow(render_data->window);
    SDL_Quit();
}
