typedef struct RenderData
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *data_texture;

    int debug_mode;
    SDL_Rect *debug_rect;
    SDL_Texture *debug_texture;

    Camera *camera;
} RenderData;

RenderData *sdl_init(int, int, int, int);
SDL_Window *sdl_create_window(int, int, int);
SDL_Renderer *sdl_create_renderer(SDL_Window *);
SDL_Texture *sdl_create_data_texture(int, int, SDL_Renderer *, SDL_Window *);
void update_data_texture_grid(RenderData *, Grid *);
void update_data_texture_tree(RenderData *, QuadTree *);
void update_data_texture_quad(uint16_t *, RenderData *, QuadTree *, Quad *);
void update_data_texture_leaf(uint16_t *, RenderData *, QuadTree *, Leaf *);
void sdl_create_debug_texture(char *, SDL_Renderer *, SDL_Texture *, SDL_Rect *);
void render(RenderData *);
void sdl_quit(RenderData *);