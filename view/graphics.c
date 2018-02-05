#include "camera.c"

#include "graphics.h"

TTF_Font *courier_new_font;
SDL_Color white_color = {255, 255, 255};

RenderData *sdl_init(int width, int height, int fullscreen, int debug_mode)
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

    render_data->window = sdl_create_window(width, height, fullscreen);
    render_data->renderer = sdl_create_renderer(render_data->window);
    render_data->data_texture = sdl_create_data_texture(width, height, render_data->renderer, render_data->window);

    render_data->debug_mode = debug_mode;

    SDL_Rect debug_rect;
    render_data->debug_rect = &debug_rect;
    render_data->debug_texture = SDL_CreateTexture(render_data->renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STATIC, 0, 0);

    uint64_t camera_x = INITIAL_QUAD_POSITION;
    camera_x = camera_x * 6 - width / 2; // Adustments on separate line because of uint32_t to uint64_t conversion.
    uint64_t camera_y = INITIAL_QUAD_POSITION;
    camera_y = camera_y * 6 - height / 2; // Adjustments on separate line because of uint32_t to uint64_t conversion.
    render_data->camera = camera_init(camera_x, camera_y, width, height);

    return render_data;
}

SDL_Window *sdl_create_window(int width, int height, int fullscreen)
{
    SDL_Window *window = SDL_CreateWindow(
        "hello_sdl2",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_SHOWN);

    if (window == NULL)
    {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        SDL_Quit();
    }

    if (fullscreen)
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        SDL_SetWindowSize(window, 2560, 1440);
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

SDL_Texture *sdl_create_data_texture(int width, int height, SDL_Renderer *renderer, SDL_Window *window)
{
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_STATIC, width, height);

    if (texture == NULL)
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        fprintf(stderr, "could not create texture: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return texture;
}

void update_data_texture_grid(RenderData *render_data, Grid *grid)
{
    int width;
    int height;

    SDL_QueryTexture(render_data->data_texture, NULL, NULL, &width, &height);

    int size = width * height;
    uint16_t *pixels = malloc(size * sizeof(*pixels));

    SDL_AtomicLock(&grid->read_lock);
    for (int i = 0; i < grid->width * grid->height; i++)
    {
        if (render_data->debug_mode)
        {
            // Show colors indicating which cells are being checked
            uint16_t state = (grid->data[i % grid->width + 1][i / grid->width + 1] & STATEMASK) / STATEMASK;
            uint16_t check = (grid->data[i % grid->width + 1][i / grid->width + 1] & CHECKMASK) / CHECKMASK;
            uint16_t color = state * 61455 + check * 255;
            pixels[i] = color;
        }
        else
        {
            // Show plain live-death state.
            pixels[i] = (grid->data[i % grid->width + 1][i / grid->width + 1] & STATEMASK) ? 61167 : 4383;
        }
    }
    SDL_AtomicUnlock(&grid->read_lock);

    SDL_UpdateTexture(render_data->data_texture, NULL, pixels, width * sizeof(uint16_t));

    free(pixels);
}

void update_data_texture_tree(RenderData *render_data, QuadTree *tree)
{
    int width;
    int height;

    SDL_QueryTexture(render_data->data_texture, NULL, NULL, &width, &height);

    int size = width * height;
    uint16_t *pixels = calloc(size, sizeof(*pixels));

    SDL_AtomicLock(&tree->read_lock);

    update_data_texture_quad(pixels, render_data, tree, tree->parent_quad);

    SDL_AtomicUnlock(&tree->read_lock);

    SDL_UpdateTexture(render_data->data_texture, NULL, pixels, width * sizeof(uint16_t));

    free(pixels);
}

void update_data_texture_quad(uint16_t *pixels, RenderData *render_data, QuadTree *tree, Quad *quad)
{
    if (camera_quad_overlap_check(render_data->camera, quad))
    {
        for (int i = 0; i < 4; i++)
        {
            if (quad->sub_quads[i] != NULL)
            {
                if (quad->level > 1)
                {
                    update_data_texture_quad(pixels, render_data, tree, quad->sub_quads[i]);
                }
                else
                {
                    update_data_texture_leaf(pixels, render_data, tree, quad->sub_quads[i]);
                }
            }
        }
    }
}

void update_data_texture_leaf(uint16_t *pixels, RenderData *render_data, QuadTree *tree, Leaf *leaf)
{
    if (camera_leaf_overlap_check(render_data->camera, leaf))
    {
        for (int block_x = 1; block_x < 7; block_x++)
        {
            for (int block_y = 1; block_y < 7; block_y++)
            {
                Block mask = 1;
                mask <<= (block_y * 8 + block_x);

                Block data = leaf->data[tree->current_gen];

                int state = (data & mask) > 0;
                int pos = leaf->x * 6 + block_x - 1 - render_data->camera->x + (leaf->y * 6 + block_y - 1 - render_data->camera->y) * render_data->camera->width;
                if (render_data->debug_mode)
                {
                    pixels[pos] = state ? 61455 : 255;
                }
                else
                {
                    pixels[pos] = state ? 61167 : 4383;
                }
            }
        }
    }
}

void update_debug_texture(char *text, RenderData *render_data)
{
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
}

void render(RenderData *render_data)
{
    SDL_RenderClear(render_data->renderer);
    SDL_RenderCopy(render_data->renderer, render_data->data_texture, NULL, NULL);
    SDL_RenderCopy(render_data->renderer, render_data->debug_texture, NULL, render_data->debug_rect);
    SDL_RenderPresent(render_data->renderer);
}

void sdl_quit(RenderData *render_data)
{
    SDL_DestroyTexture(render_data->data_texture);
    SDL_DestroyTexture(render_data->debug_texture);
    SDL_DestroyRenderer(render_data->renderer);
    SDL_DestroyWindow(render_data->window);
    SDL_Quit();
}
