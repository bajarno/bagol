// Struct representing grid data.
typedef struct Grid {
    int width;
    int height;
    char ** data;

    // SpinLock for reading and writing to data, used for blocking access by other threads.
    SDL_SpinLock write_lock;
    SDL_SpinLock read_lock;
} Grid;

Grid grid_init(int , int);
void free_grid_memory(Grid*);
void grid_step(Grid*);
void grid_clear(Grid*);