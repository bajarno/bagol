#include "pattern.h"

void add_pattern(Pattern pattern, Grid *grid, int x, int y)
{
    if (pattern != RPENTOMINO_SPAM)
    {
        SDL_AtomicLock(&grid->write_lock);
    }

    switch (pattern)
    {
    case BLINKER:
        set_state(grid, x + 0, y, 1);
        set_state(grid, x + 1, y, 1);
        set_state(grid, x + 1, y, 1);
        break;

    case GLIDER:
        set_state(grid, x + 0, y + 1, 1);
        set_state(grid, x + 1, y + 2, 1);
        set_state(grid, x + 2, y + 0, 1);
        set_state(grid, x + 2, y + 1, 1);
        set_state(grid, x + 2, y + 2, 1);
        break;

    case RPENTOMINO:
        set_state(grid, x + 1, y + 0, 1);
        set_state(grid, x + 2, y + 0, 1);
        set_state(grid, x + 0, y + 1, 1);
        set_state(grid, x + 1, y + 1, 1);
        set_state(grid, x + 1, y + 2, 1);
        break;

    case RPENTOMINO_SPAM:
        for (int x = 0; x < grid->width / 40; x++)
        {
            for (int y = 0; y < grid->height / 40; y++)
            {
                add_pattern(RPENTOMINO, grid, x * 40 + 19 + rand() % 3, y * 40 + 19 + rand() % 3);
            }
        }
        break;
    }

    if (pattern != RPENTOMINO_SPAM)
    {
        SDL_AtomicUnlock(&grid->write_lock);
    }
}

void set_state(Grid *grid, int x, int y, uint8_t state)
{
    grid->data[x][y] &= STATEUNMASK;
    grid->data[x][y] |= state * STATEMASK;

    if (grid->algorithm == BASIC_DIFF || grid->algorithm == NEIGHBOURS_DIFF)
    {
        grid->data[x][y] |= CHECKMASK;

        grid->data[x - 1][y - 1] |= CHECKMASK;
        grid->data[x - 1][y] |= CHECKMASK;
        grid->data[x - 1][y + 1] |= CHECKMASK;
        grid->data[x][y - 1] |= CHECKMASK;
        grid->data[x][y + 1] |= CHECKMASK;
        grid->data[x + 1][y - 1] |= CHECKMASK;
        grid->data[x + 1][y] |= CHECKMASK;
        grid->data[x + 1][y + 1] |= CHECKMASK;
    }

    if (grid->algorithm == NEIGHBOURS || grid->algorithm == NEIGHBOURS_DIFF)
    {
        uint8_t delta = state ? 1 : -1;

        grid->data[x - 1][y - 1] += delta;
        grid->data[x - 1][y] += delta;
        grid->data[x - 1][y + 1] += delta;
        grid->data[x][y - 1] += delta;
        grid->data[x][y + 1] += delta;
        grid->data[x + 1][y - 1] += delta;
        grid->data[x + 1][y] += delta;
        grid->data[x + 1][y + 1] += delta;
    }
}
