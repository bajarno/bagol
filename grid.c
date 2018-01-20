#include "grid.h"

// Initialize grid struct instance
Grid grid_init(int width, int height) {
    Grid grid;

    grid.width = width;
    grid.height = height;

    // Set lock values to 0 (unlocked)
    grid.write_lock = 0;
    grid.read_lock = 0;

    // Allocate memory
    grid.data = malloc(width * sizeof(*grid.data));
    for(int i = 0; i < width; i++) {
        grid.data[i] = malloc(height * sizeof(*(grid.data[i])));
    }
    grid_clear(&grid);


    return grid;
}

// Free memory of grid data.
void free_grid_memory(Grid * grid) {
    for (int i = 0; i < grid->width; i++) {
        free(grid->data[i]);
    }
    free(grid->data);
}

// Calculates next generation and updates grid data.
void grid_step(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);

    // New data matrix
    char ** new_data = malloc(grid->width * sizeof(*new_data));
    for (int i = 0; i < grid->width; i++) {
        new_data[i] = malloc(grid->height * sizeof(*(new_data[i])));
    }

    // Calculate new values
    for (int x = 0; x < grid->width; x++) {
        for (int y = 0; y < grid->height; y++) {
            int living = 0;

            int x_before = (x + grid->width - 1) % grid->width;
            int x_after = (x + 1) % grid->width;
            int y_before = (y + grid->height - 1) % grid->height;
            int y_after = (y + 1) % grid->height;

            living += grid->data[x_before][y_before];
            living += grid->data[x_before][y];
            living += grid->data[x_before][y_after];
            living += grid->data[x][y_before];
            living += grid->data[x][y_after];
            living += grid->data[x_after][y_before];
            living += grid->data[x_after][y];
            living += grid->data[x_after][y_after];
            
            // If two living neighbours, cell will keep current state
            if (living == 2) {
                new_data[x][y] = grid->data[x][y];
            // If three living neighbours, cell will live. Less than two or more than three results in death.
            } else {
                new_data[x][y] = living == 3;
            }
        }
    }
    
    // Free memory of previous data matrix
    SDL_AtomicLock(&grid->read_lock);
    free_grid_memory(grid);
    grid->data = new_data;
    SDL_AtomicUnlock(&grid->read_lock);
    
    SDL_AtomicUnlock(&grid->write_lock);
}

// Set whole grid to empty cells.
void grid_clear(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);

    for (int x = 0; x < grid->width; x++) {
        for (int y = 0; y < grid->height; y++) {
            grid->data[x][y] = 0;
        }
    }

    SDL_AtomicUnlock(&grid->write_lock);
}