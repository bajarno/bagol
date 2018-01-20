#include "pattern.h"

void add_pattern(Pattern pattern, Grid * grid, int x, int y) {
    SDL_AtomicLock(&grid->write_lock);
    switch (pattern) {
        case BLINKER:
            grid->data[x+0][y] = 1;
            grid->data[x+1][y] = 1;
            grid->data[x+2][y] = 1;
            break;

        case GLIDER:
            grid->data[x+0][y+1] = 1;
            grid->data[x+1][y+2] = 1;
            grid->data[x+2][y+0] = 1;
            grid->data[x+2][y+1] = 1;
            grid->data[x+2][y+2] = 1;
            break;

        case RPENTOMINO:
            grid->data[x+1][y+0] = 1;
            grid->data[x+2][y+0] = 1;
            grid->data[x+0][y+1] = 1;
            grid->data[x+1][y+1] = 1;
            grid->data[x+1][y+2] = 1;
            break;
    }
    SDL_AtomicUnlock(&grid->write_lock);
}