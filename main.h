// Struct representing draw data.
typedef struct AppData {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    Grid* grid;

    short quit;
} AppData;

int draw_loop(void*);
int event_loop(void*);
int update_loop(void*);