void grid_step_neighbours(Grid *grid)
{
    SDL_AtomicLock(&grid->write_lock);
    uint8_t **new_data = grid->data_prev;

    for (int x = 1; x < (grid->width + 1); x++)
    {
        memcpy(new_data[x], grid->data[x], (grid->height + 2) * sizeof(*(grid->data[x])));
    }

    // Calculate new values
    for (int x = 1; x < (grid->width + 1); x++)
    {
        for (int y = 1; y < (grid->height + 1); y++)
        {
            int living = grid->data[x][y] & NEIGHBOURSMASK;

            int new_state;

            // If two living neighbours, cell will keep current state
            if (living == 2)
            {
                new_state = grid->data[x][y] & STATEMASK;
                // If three living neighbours, cell will live. Less than two or more than three results in death.
            }
            else
            {
                new_state = (living == 3) * STATEMASK;
            }

            // Value needs to be updated
            if (new_state != (grid->data[x][y] & STATEMASK))
            {
                uint8_t delta = new_state ? 1 : -1;

                new_data[x - 1][y - 1] += delta;
                new_data[x - 1][y] += delta;
                new_data[x - 1][y + 1] += delta;
                new_data[x][y - 1] += delta;
                new_data[x][y + 1] += delta;
                new_data[x + 1][y - 1] += delta;
                new_data[x + 1][y] += delta;
                new_data[x + 1][y + 1] += delta;

                new_data[x][y] &= STATEUNMASK;
                new_data[x][y] |= new_state;
            }
        }
    }

    // Fix data on the edge in case of a cyclic grid
    if (grid->cyclic)
    {
        for (int x = 1; x < (grid->width + 1); x++)
        {
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
