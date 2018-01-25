#include "grid.h"

// Initialize grid struct instance
Grid grid_init(int width, int height, int cyclic, Algorithm algorithm) {
    Grid grid;

    grid.width = width;
    grid.height = height;
    grid.cyclic = cyclic;
    grid.algorithm = algorithm;

    // Set lock values to 0 (unlocked)
    grid.write_lock = 0;
    grid.read_lock = 0;

    // Allocate memory
    grid.data = malloc((width + 2) * sizeof(*grid.data));
    for(int i = 1; i < (width + 1); i++) {
        grid.data[i] = malloc((height + 2) * sizeof(*(grid.data[i])));
    }

    grid.data_prev = malloc((width + 2) * sizeof(*grid.data_prev));
    for(int i = 1; i < (width + 1); i++) {
        grid.data_prev[i] = malloc((height + 2) * sizeof(*(grid.data_prev[i])));
    }

    if (cyclic) {
        grid.data[0] = grid.data[width];
        grid.data[width+1] = grid.data[1];

        grid.data_prev[0] = grid.data_prev[width];
        grid.data_prev[width+1] = grid.data_prev[1];
    } else {
        grid.data[0] = malloc((height + 2) * sizeof(*(grid.data[0])));
        grid.data[width+1] = malloc((height + 2) * sizeof(*(grid.data[0])));

        grid.data_prev[0] = malloc((height + 2) * sizeof(*(grid.data[0])));
        grid.data_prev[width+1] = malloc((height + 2) * sizeof(*(grid.data[0])));
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
        case BASIC_DIFF:
            grid_step_basic_diff(grid);
            break;
        case NEIGHBOURS:
            grid_step_neighbours(grid);
            break;
    }
}

// Calculates next generation and updates grid data.
void grid_step_basic(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);
    uint8_t ** new_data = grid->data_prev;

    // Calculate new values
    for (int x = 1; x < (grid->width + 1); x++) {
        for (int y = 1; y < (grid->height + 1); y++) {
            new_data[x][y] = get_next_state_basic(grid->data, x, y);
        }

        // Fix data on the edge in case of a cyclic grid
        if (grid->cyclic) {
            new_data[x][0] = new_data[x][grid->height];
            new_data[x][grid->height + 1] = new_data[x][1];
        }
    }
    
    SDL_AtomicLock(&grid->read_lock);
    grid->data_prev = grid->data;
    grid->data = new_data;
    SDL_AtomicUnlock(&grid->read_lock);
    
    SDL_AtomicUnlock(&grid->write_lock);
}


void grid_step_basic_diff(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);
    uint8_t ** new_data = grid->data_prev;

    for (int x = 1; x < (grid->width + 1); x++) {
        memcpy(new_data[x], grid->data[x], (grid->height + 2) * sizeof(*(grid->data[x])));
    }

    // Calculate new values
    for (int x = 1; x < (grid->width + 1); x++) {
        for (int y = 1; y < (grid->height + 1); y++) {
            if (grid->data[x][y] & CHECKMASK) {
                uint8_t new_state = get_next_state_basic(grid->data, x, y);

                if (new_state != (grid->data[x][y] & STATEMASK)) {
                    new_data[x][y] |= CHANGEMASK;

                    new_data[x][y] &= STATEUNMASK;
                    new_data[x][y] |= new_state;
                    
                    new_data[x-1][y-1] |= CHECKMASK;
                    new_data[x-1][y] |= CHECKMASK;
                    new_data[x-1][y+1] |= CHECKMASK;
                    new_data[x][y-1] |= CHECKMASK;
                    new_data[x][y+1] |= CHECKMASK;
                    new_data[x+1][y-1] |= CHECKMASK;
                    new_data[x+1][y] |= CHECKMASK;
                    new_data[x+1][y+1] |= CHECKMASK;
                } else {
                    new_data[x][y] &= CHANGEUNMASK;

                    uint8_t neighbours_changed = 
                        (new_data[x-1][y-1] |
                        new_data[x-1][y] |
                        new_data[x-1][y+1] |
                        new_data[x][y-1] |
                        new_data[x][y+1] |
                        new_data[x+1][y-1] |
                        new_data[x+1][y] |
                        new_data[x+1][y+1]) &
                        CHANGEMASK;
                    
                    if (!neighbours_changed) {
                        new_data[x][y] &= CHECKUNMASK;
                    }
                }
            }
        }
    }

    if (grid->cyclic) {
        for (int x = 1; x < (grid->width + 1); x++) {
            // Fix data on the edge in case of a cyclic grid
            new_data[x][grid->height] |= new_data[x][0] & CHECKMASK;
            new_data[x][0] = new_data[x][grid->height];

            new_data[x][1] |= new_data[x][grid->height + 1] & CHECKMASK;
            new_data[x][grid->height + 1] = new_data[x][1];
        }
    }
    
    SDL_AtomicLock(&grid->read_lock);
    grid->data_prev = grid->data;
    grid->data = new_data;
    SDL_AtomicUnlock(&grid->read_lock);
    
    SDL_AtomicUnlock(&grid->write_lock);
}

uint8_t get_next_state_basic(uint8_t ** data, int x, int y) {
    int living = 0;

    living += data[x-1][y-1] & STATEMASK;
    living += data[x-1][y] & STATEMASK;
    living += data[x-1][y+1] & STATEMASK;
    living += data[x][y-1] & STATEMASK;
    living += data[x][y+1] & STATEMASK;
    living += data[x+1][y-1] & STATEMASK;
    living += data[x+1][y] & STATEMASK;
    living += data[x+1][y+1] & STATEMASK;

    uint8_t new_state;

    // If two living neighbours, cell will keep current state
    if (living == (2 * STATEMASK)) {
        new_state = data[x][y] & STATEMASK;
    // If three living neighbours, cell will live. Less than two or more than three results in death.
    } else {
        new_state = ((living == (3 * STATEMASK)) * STATEMASK);
    }

    return new_state;
}

void grid_step_neighbours(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);
    uint8_t ** new_data = grid->data_prev;

    for (int x = 1; x < (grid->width + 1); x++) {
        memcpy(new_data[x], grid->data[x], (grid->height + 2) * sizeof(*(grid->data[x])));
    }    

    // Calculate new values
    for (int x = 1; x < (grid->width + 1); x++) {
        for (int y = 1; y < (grid->height + 1); y++) {
            int living = grid->data[x][y] & NEIGHBOURSMASK;
            
            int new_state;

            // If two living neighbours, cell will keep current state
            if (living == 2) {
                new_state = grid->data[x][y] & STATEMASK;
            // If three living neighbours, cell will live. Less than two or more than three results in death.
            } else {
                new_state = (living == 3) * STATEMASK;
            }

            // Value needs to be updated
            if (new_state != (grid->data[x][y] & STATEMASK)) {
                uint8_t delta = new_state ? 1 : -1;

                new_data[x-1][y-1] += delta;
                new_data[x-1][y] += delta;
                new_data[x-1][y+1] += delta;
                new_data[x][y-1] += delta;
                new_data[x][y+1] += delta;
                new_data[x+1][y-1] += delta;
                new_data[x+1][y] += delta;
                new_data[x+1][y+1] += delta;

                new_data[x][y] &= STATEUNMASK;
                new_data[x][y] |= new_state;
            }
        }
    }

    // Fix data on the edge in case of a cyclic grid
    if (grid->cyclic) {
        for (int x = 1; x < (grid->width + 1); x++) {
            new_data[x][grid->height] += new_data[x][0] - grid->data[x][0];
            new_data[x][0] = new_data[x][grid->height];

            new_data[x][1] += new_data[x][grid->height + 1] - grid->data[x][grid->height + 1];
            new_data[x][grid->height + 1] = new_data[x][1];
        }
    }
    
    SDL_AtomicLock(&grid->read_lock);
    grid->data_prev = grid->data;
    grid->data = new_data;
    SDL_AtomicUnlock(&grid->read_lock);
    
    SDL_AtomicUnlock(&grid->write_lock);
}

// Set whole grid to empty cells.
void grid_clear(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);

    for (int x = 0; x < (grid->width + 2); x++) {
        for (int y = 0; y < (grid->height + 2); y++) {
            grid->data[x][y] = 0;
            grid->data_prev[x][y] = 0;
        }
    }

    SDL_AtomicUnlock(&grid->write_lock);
}
