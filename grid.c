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

void free_grid_memory(Grid grid) {
    for (int i = 0; i < grid.width; i++) {
        free(grid.data[i]);
    }
    free(grid.data);
}

Grid grid_step(Grid grid) {
    // char *ext_data[grid.width + 2];
    // ext_data[0] = grid.data[grid.width-1];
    // for (int i = 1; i < grid.width + 1; i++) {
    //     ext_data[i] = grid.data[i-1];
    // }
    // ext_data[grid.width-1] = grid.data[0];

    char ** new_data = malloc(grid.width * sizeof(*new_data));
    for (int i = 0; i < grid.width; i++) {
        new_data[i] = malloc(grid.height * sizeof(*(new_data[i])));
    }

    for (int x = 0; x < grid.width; x++) {
        for (int y = 0; y < grid.height; y++) {
            int living = 0;

            int x_before = (x + grid.width - 1) % grid.width;
            int x_after = (x + 1) % grid.width;
            int y_before = (y + grid.height - 1) % grid.height;
            int y_after = (y + 1) % grid.height;

            living += grid.data[x_before][y_before];
            living += grid.data[x_before][y];
            living += grid.data[x_before][y_after];
            living += grid.data[x][y_before];
            living += grid.data[x][y_after];
            living += grid.data[x_after][y_before];
            living += grid.data[x_after][y];
            living += grid.data[x_after][y_after];
            
            if (living < 2 || living > 3) {
                new_data[x][y] = 0;
            } else if (living == 3) {
                new_data[x][y] = 1;
            } else {
                new_data[x][y] = grid.data[x][y]*1;
            }
        }
    }
    
    free_grid_memory(grid);

    grid.data = new_data;

    return grid;
}

void grid_clear(Grid grid) {
    for (int x = 0; x < grid.width; x++) {
        for (int y = 0; y < grid.height; y++) {
            grid.data[x][y] = 0;
        }
    }
}