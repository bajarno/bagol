#include "grid.h"

uint8_t ** temp_data;
uint8_t ** temp_metadata;


// Initialize grid struct instance
Grid grid_init(int width, int height) {
    Grid grid;

    grid.width = width;
    grid.height = height;
    grid.algorithm = NEIGHBOURS;

    // Set lock values to 0 (unlocked)
    grid.write_lock = 0;
    grid.read_lock = 0;

    // Allocate memory
    grid.data = malloc(width * sizeof(*grid.data));
    for(int i = 0; i < width; i++) {
        grid.data[i] = malloc(height * sizeof(*(grid.data[i])));
    }

    temp_data = malloc(width * sizeof(*temp_data));
    for(int i = 0; i < width; i++) {
        temp_data[i] = malloc(height * sizeof(*(temp_data[i])));
    }

    if (grid.algorithm == NEIGHBOURS) {
        // Allo e memory for previous generation data
        grid.metadata = malloc(width * sizeof(*grid.metadata));
        for(int i = 0; i < width; i++) {
            grid.metadata[i] = malloc(height * sizeof(*(grid.metadata[i])));
        }

        // Allocate memory for metadata
        temp_metadata = malloc(width * sizeof(*temp_metadata));
        for(int i = 0; i < width; i++) {
            temp_metadata[i] = malloc(height * sizeof(*(temp_metadata[i])));
        }
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

void grid_step(Grid * grid) {
    switch (grid->algorithm) {
        case BASIC:
            grid_step_basic(grid);
            break;
        case NEIGHBOURS:
            grid_step_neighbours(grid);
            break;
    }
}

// Calculates next generation and updates grid data.
void grid_step_basic(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);
    uint8_t ** new_data = temp_data;
    
    // Calculate new values
    for (int x = 0; x < grid->width; x++) {
        for (int y = 0; y < grid->height; y++) {
            int living = 0;

            // Calculate inline for performance
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
    
    SDL_AtomicLock(&grid->read_lock);
    temp_data = grid->data;
    grid->data = new_data;
    SDL_AtomicUnlock(&grid->read_lock);
    
    SDL_AtomicUnlock(&grid->write_lock);
}

void grid_step_neighbours(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);
    uint8_t ** new_data = temp_data;
    uint8_t ** new_metadata = temp_metadata;

    // Remove these 4 lines for super trippy effect
    for (int i = 0; i < grid->width; i++) {
        memcpy(new_data[i], grid->data[i], grid->height);
        memcpy(new_metadata[i], grid->metadata[i], grid->height);
    }
    
    // Calculate new values
    for (int x = 0; x < grid->width; x++) {
        for (int y = 0; y < grid->height; y++) {
            int living = grid->metadata[x][y];

            int new_state;

            // If two living neighbours, cell will keep current state
            if (living == 2) {
                new_state = grid->data[x][y];
            // If three living neighbours, cell will live. Less than two or more than three results in death.
            } else {
                new_state = living == 3;
            }

            // Value needs to be updated
            if (new_state != grid->data[x][y]) {
                // Calculate inline for performance
                int x_before = (x + grid->width - 1) % grid->width;
                int x_after = (x + 1) % grid->width;
                int y_before = (y + grid->height - 1) % grid->height;
                int y_after = (y + 1) % grid->height;

                uint8_t delta = new_state ? 1 : -1;

                new_metadata[x_before][y_before] += delta;
                new_metadata[x_before][y] += delta;
                new_metadata[x_before][y_after] += delta;
                new_metadata[x][y_before] += delta;
                new_metadata[x][y_after] += delta;
                new_metadata[x_after][y_before] += delta;
                new_metadata[x_after][y] += delta;
                new_metadata[x_after][y_after] += delta;

                new_data[x][y] = new_state;
            }
        }
    }
    
    // // Free memory of previous data matrix
    SDL_AtomicLock(&grid->read_lock);
    temp_data = grid->data;
    temp_metadata = grid->metadata;
    grid->data = new_data;
    grid->metadata = new_metadata;
    SDL_AtomicUnlock(&grid->read_lock);
    
    SDL_AtomicUnlock(&grid->write_lock);
}

// Set whole grid to empty cells.
void grid_clear(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);

    for (int x = 0; x < grid->width; x++) {
        for (int y = 0; y < grid->height; y++) {
            grid->data[x][y] = 0;

            if (grid->algorithm == NEIGHBOURS) {
                grid->metadata[x][y] = 0;
            }
        }
    }

    SDL_AtomicUnlock(&grid->write_lock);
}