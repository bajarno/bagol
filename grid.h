typedef enum Algorithm {
    // Most basic implementation. Checks all neighbours for each cell each generation.
    // metadata0: Unused
    // metadata1: Unused
    BASIC,

    // Keeps track of amount of living neighbours for each cell.
    // metadata0: Contains amount of neighbours
    // metadata1: Contains amount of neighbours for previous generation
    NEIGHBOURS
} Algorithm;

// Struct representing grid data.
typedef struct Grid {
    int width;
    int height;

    // The algorithm that is used for calculating new generations.
    Algorithm algorithm;

    // Represents the generation, 1 = alive, 0 = death.
    uint8_t ** data;

    // Contains metadata for each cell, exact use depends on algorithm.
    uint8_t ** metadata;

    // SpinLock for reading and writing to data, used for blocking access by other threads.
    SDL_SpinLock write_lock;
    SDL_SpinLock read_lock;
} Grid;

Grid grid_init(int , int);
void free_grid_memory(Grid*);
void grid_step(Grid*);
void grid_step_basic(Grid*);
void grid_step_neighbours(Grid*);
void grid_clear(Grid*);