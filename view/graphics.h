typedef struct RenderData
{
    SDL_Window *window;
    int window_width;
    int window_height;

    SDL_Renderer *renderer;
    SDL_Texture *data_texture;

    int debug_mode;
    SDL_Rect *debug_rect;
    SDL_Texture *debug_texture;

    Camera *camera;

    // SpinLock for reading and writing to data, used for blocking access by other threads.
    SDL_SpinLock write_lock;
} RenderData;

RenderData *sdl_init(int, int, int, int);
SDL_Window *sdl_create_window(int, int, int);
SDL_Renderer *sdl_create_renderer(SDL_Window *);
void sdl_create_debug_texture(char *, SDL_Renderer *, SDL_Texture *, SDL_Rect *);
void render(RenderData *);
void sdl_quit(RenderData *);