void sdl_init();
SDL_Window* sdl_create_window(int, int, int);
SDL_Renderer* sdl_create_renderer(SDL_Window*);
SDL_Texture* sdl_create_texture(int, int, SDL_Renderer*, SDL_Window*);
void update_grid_texture(SDL_Texture*, Grid*);
void sdl_create_debug_texture(char*, SDL_Renderer*, SDL_Texture*, SDL_Rect*);
void render(AppData*);
void sdl_quit(AppData*);