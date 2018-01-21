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
        grid->data[x][y] = state;
    } else if (grid->algorithm == NEIGHBOURS) {
        int x_before = (x + grid->width - 1) % grid->width;
        int x_after = (x + 1) % grid->width;
        int y_before = (y + grid->height - 1) % grid->height;
        int y_after = (y + 1) % grid->height;

        uint8_t delta = state ? 1 : -1;

        grid->data[x][y] = state;

        grid->metadata[x_before][y_before] += delta;
        grid->metadata[x_before][y] += delta;
        grid->metadata[x_before][y_after] += delta;
        grid->metadata[x][y_before] += delta;
        grid->metadata[x][y_after] += delta;
        grid->metadata[x_after][y_before] += delta;
        grid->metadata[x_after][y] += delta;
        grid->metadata[x_after][y_after] += delta;
    }
}