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
                int living = 0;

                living += grid->data[x-1][y-1] & STATEMASK;
                living += grid->data[x-1][y] & STATEMASK;
                living += grid->data[x-1][y+1] & STATEMASK;
                living += grid->data[x][y-1] & STATEMASK;
                living += grid->data[x][y+1] & STATEMASK;
                living += grid->data[x+1][y-1] & STATEMASK;
                living += grid->data[x+1][y] & STATEMASK;
                living += grid->data[x+1][y+1] & STATEMASK;

                uint8_t new_state;

                // If two living neighbours, cell will keep current state
                if (living == (2 * STATEMASK)) {
                    new_state = grid->data[x][y] & STATEMASK;
                // If three living neighbours, cell will live. Less than two or more than three results in death.
                } else {
                    new_state = ((living == (3 * STATEMASK)) * STATEMASK);
                }

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
