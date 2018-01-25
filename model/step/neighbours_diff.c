void grid_step_neighbours_diff(Grid * grid) {
    SDL_AtomicLock(&grid->write_lock);
    uint8_t ** new_data = grid->data_prev;

    for (int x = 1; x < (grid->width + 1); x++) {
        memcpy(new_data[x], grid->data[x], (grid->height + 2) * sizeof(*(grid->data[x])));

        // Set the check bits of the duplicate cells to zero. These have no function after the
        // content is copied at the end of a step and will always stay at one otherwise.
        new_data[x][0] &= CHECKUNMASK;
        new_data[x][grid->height+1] &= CHECKUNMASK;
    }

    // Calculate new values
    for (int x = 1; x < (grid->width + 1); x++) {
        for (int y = 1; y < (grid->height + 1); y++) {
            if (grid->data[x][y] & CHECKMASK) {
                int living = grid->data[x][y] & NEIGHBOURSMASK;
                
                int new_state;

                // If two living neighbours, cell will keep current state
                if (living == 2) {
                    new_state = grid->data[x][y] & STATEMASK;
                // If three living neighbours, cell will live. Less than two or more than three results in death.
                } else {
                    new_state = (living == 3) * STATEMASK;
                }

                if (new_state != (grid->data[x][y] & STATEMASK)) {
                    // Set new state
                    new_data[x][y] &= STATEUNMASK;
                    new_data[x][y] |= new_state;

                    // Indicate change in value
                    new_data[x][y] |= CHANGEMASK;

                    // Adjust neighbour count in neighbouring cells
                    uint8_t delta = new_state ? 1 : -1;

                    new_data[x-1][y-1] += delta;
                    new_data[x-1][y] += delta;
                    new_data[x-1][y+1] += delta;
                    new_data[x][y-1] += delta;
                    new_data[x][y+1] += delta;
                    new_data[x+1][y-1] += delta;
                    new_data[x+1][y] += delta;
                    new_data[x+1][y+1] += delta;

                    // Indicate that neighbouring cells need to be checked in the next generation.
                    new_data[x-1][y-1] |= CHECKMASK;
                    new_data[x-1][y] |= CHECKMASK;
                    new_data[x-1][y+1] |= CHECKMASK;
                    new_data[x][y-1] |= CHECKMASK;
                    new_data[x][y+1] |= CHECKMASK;
                    new_data[x+1][y-1] |= CHECKMASK;
                    new_data[x+1][y] |= CHECKMASK;
                    new_data[x+1][y+1] |= CHECKMASK;
                } else {
                    // Indicate no change in value
                    new_data[x][y] &= CHANGEUNMASK;

                    // Check whether on of the neighbours changed. If not, this cell does not need to
                    // be checked in the next generation.
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
        // Get the mirrored cells back in sync with the cell they represent.
        for (int x = 1; x < (grid->width + 1); x++) {
            new_data[x][grid->height] |= (new_data[x][0] & CHECKMASK);
            new_data[x][grid->height] += (new_data[x][0] & NEIGHBOURSMASK) - (grid->data[x][0] & NEIGHBOURSMASK);
            new_data[x][0] = new_data[x][grid->height];

            new_data[x][1] |= (new_data[x][grid->height + 1] & CHECKMASK);
            new_data[x][1] += (new_data[x][grid->height + 1] & NEIGHBOURSMASK) - (grid->data[x][grid->height + 1] & NEIGHBOURSMASK);
            new_data[x][grid->height + 1] = new_data[x][1];
        }
    }
    
    SDL_AtomicLock(&grid->read_lock);
    grid->data_prev = grid->data;
    grid->data = new_data;
    SDL_AtomicUnlock(&grid->read_lock);
    
    SDL_AtomicUnlock(&grid->write_lock);
}
