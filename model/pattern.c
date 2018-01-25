#include "pattern.h"

void add_pattern(Pattern pattern, Grid * grid, int x, int y) {
    SDL_AtomicLock(&grid->write_lock);
    switch (pattern) {
        case BLINKER:
            set_state(grid, x+0, y, 1);
            set_state(grid, x+1, y, 1);
            set_state(grid, x+1, y, 1);
            break;

        case GLIDER:
            set_state(grid, x+0, y+1, 1);
            set_state(grid, x+1, y+2, 1);
            set_state(grid, x+2, y+0, 1);
            set_state(grid, x+2, y+1, 1);
            set_state(grid, x+2, y+2, 1);
            break;

        case RPENTOMINO:
            set_state(grid, x+1, y+0, 1);
            set_state(grid, x+2, y+0, 1);
            set_state(grid, x+0, y+1, 1);
            set_state(grid, x+1, y+1, 1);
            set_state(grid, x+1, y+2, 1);
            break;
    }
    SDL_AtomicUnlock(&grid->write_lock);
}

void set_state(Grid * grid, int x, int y, uint8_t state) {
    if (grid->algorithm == BASIC) {
        grid->data[x][y] = state * STATEMASK;
    } else if (grid->algorithm == BASIC_DIFF) {
        grid->data[x][y] |= (STATEMASK | CHECKMASK);

        grid->data[x-1][y-1] |= CHECKMASK;
        grid->data[x-1][y] |= CHECKMASK;
        grid->data[x-1][y+1] |= CHECKMASK;
        grid->data[x][y-1] |= CHECKMASK;
        grid->data[x][y+1] |= CHECKMASK;
        grid->data[x+1][y-1] |= CHECKMASK;
        grid->data[x+1][y] |= CHECKMASK;
        grid->data[x+1][y+1] |= CHECKMASK;
    } else if (grid->algorithm == NEIGHBOURS) {
        uint8_t delta = state ? 1 : -1;
    
        grid->data[x][y] &= STATEUNMASK;
        grid->data[x][y] |= state * STATEMASK;

        grid->data[x-1][y-1] += delta;
        grid->data[x-1][y] += delta;
        grid->data[x-1][y+1] += delta;
        grid->data[x][y-1] += delta;
        grid->data[x][y+1] += delta;
        grid->data[x+1][y-1] += delta;
        grid->data[x+1][y] += delta;
        grid->data[x+1][y+1] += delta;
    } else if (grid->algorithm == NEIGHBOURS_DIFF) {
        uint8_t delta = state ? 1 : -1;
    
        grid->data[x][y] &= STATEUNMASK;
        grid->data[x][y] |= state * STATEMASK;

        grid->data[x][y] |= (STATEMASK | CHECKMASK);

        grid->data[x-1][y-1] += delta;
        grid->data[x-1][y] += delta;
        grid->data[x-1][y+1] += delta;
        grid->data[x][y-1] += delta;
        grid->data[x][y+1] += delta;
        grid->data[x+1][y-1] += delta;
        grid->data[x+1][y] += delta;
        grid->data[x+1][y+1] += delta;

        grid->data[x-1][y-1] |= CHECKMASK;
        grid->data[x-1][y] |= CHECKMASK;
        grid->data[x-1][y+1] |= CHECKMASK;
        grid->data[x][y-1] |= CHECKMASK;
        grid->data[x][y+1] |= CHECKMASK;
        grid->data[x+1][y-1] |= CHECKMASK;
        grid->data[x+1][y] |= CHECKMASK;
        grid->data[x+1][y+1] |= CHECKMASK;
    }
}
