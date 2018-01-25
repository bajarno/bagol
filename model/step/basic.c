void grid_step_basic(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);

    uint8_t ** new_data = grid->data_prev;

    // Calculate new values
    for (int x = 1; x < (grid->width + 1); x++) {
        for (int y = 1; y < (grid->height + 1); y++) {
            int living = 0;

            living += grid->data[x-1][y-1];
            living += grid->data[x-1][y];
            living += grid->data[x-1][y+1];
            living += grid->data[x][y-1];
            living += grid->data[x][y+1];
            living += grid->data[x+1][y-1];
            living += grid->data[x+1][y];
            living += grid->data[x+1][y+1];

            // If two living neighbours, cell will keep current state
            if (living == (2 * STATEMASK)) {
                new_data[x][y] = grid->data[x][y];
            // If three living neighbours, cell will live. Less than two or more than three results in death.
            } else {
                new_data[x][y] = ((living == (3 * STATEMASK)) * STATEMASK);
            }
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
