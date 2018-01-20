// Struct representing draw data.
typedef struct AppData {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* grid_texture;

    SDL_Rect* debug_rect;
    SDL_Texture* debug_texture;
    Grid* grid;

    float fps;
    float ups;

    short quit;
} AppData;

int draw_loop(void*);
int event_loop(void*);
int update_loop(void*);