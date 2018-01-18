typedef struct {
    int width;
    int height;
    char ** data;
} Grid;

Grid grid_init(int width, int height) {
    Grid grid;

    grid.width = width;
    grid.height = height;

    grid.data = malloc(width * sizeof(*grid.data));
    for(int i = 0; i < width; i++) {
        grid.data[i] = malloc(height * sizeof(*(grid.data[i])));
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            grid.data[x][y] = 0;
        }
    }

    return grid;
}

void free_grid(Grid grid) {
    for (int i = 0; i < grid.width; i++) {
        free(grid.data[i]);
    }
    free(grid.data);
}

Grid grid_step(Grid grid) {
    char ** new_data = malloc(grid.width * sizeof(*new_data));
    for(int i = 0; i < grid.width; i++) {
        new_data[i] = malloc(grid.height * sizeof(*(new_data[i])));
    }

    for (int x = 0; x < grid.width; x++) {
        for (int y = 0; y < grid.height; y++) {
            int living = 0;
            
            if (x > 0) {
                living += grid.data[x-1][y];
            }
            if (y > 0) {
                living += grid.data[x][y-1];
            }
            if (x < grid.width - 1) {
                living += grid.data[x+1][y];
            }
            if (y < grid.height - 1) {
                living += grid.data[x][y+1];
            }
            if (x > 0 && y > 0) {
                living += grid.data[x-1][y-1];
            }
            if (x > 0 && y < grid.height -1) {
                living += grid.data[x-1][y+1];
            }
            if (x < grid.width - 1 && y > 0) {
                living += grid.data[x+1][y-1];
            }
            if (x < grid.width -1 && y < grid.height - 1) {
                living += grid.data[x+1][y+1];
            }

            if (living < 2 || living > 3) {
                new_data[x][y] = 0;
            } else if (living == 3) {
                new_data[x][y] = 1;
            } else {
                new_data[x][y] = grid.data[x][y]*1;
            }
        }
    }
    
    free_grid(grid);

    grid.data = new_data;

    return grid;
}