typedef enum {
    // Oscillators
    Blinker,

    // Spaceships
    Glider,

    // Other
    RPENTOMINO
} Pattern;

void add_pattern(Pattern pattern, Grid grid, int x, int y) {
    switch (pattern) {
        case Blinker:
            grid.data[x+0][y] = 1;
            grid.data[x+1][y] = 1;
            grid.data[x+2][y] = 1;
            break;

        case Glider:
            grid.data[x+1][y+0] = 1;
            grid.data[x+2][y+1] = 1;
            grid.data[x+0][y+2] = 1;
            grid.data[x+1][y+3] = 1;
            grid.data[x+2][y+3] = 1;
            break;

        case RPENTOMINO:
            grid.data[x+1][y+0] = 1;
            grid.data[x+2][y+0] = 1;
            grid.data[x+0][y+1] = 1;
            grid.data[x+1][y+1] = 1;
            grid.data[x+1][y+2] = 1;
            break;
    }
}