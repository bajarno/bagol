#include "grid.h"
#include "step/basic.c"
#include "step/basic_diff.c"
#include "step/neighbours.c"
#include "step/neighbours_diff.c"

// Initialize grid struct instance
Grid grid_init(int width, int height, int cyclic, Algorithm algorithm)
{
    Grid grid;

    grid.width = width;
    grid.height = height;
    grid.cyclic = cyclic;
    grid.algorithm = algorithm;
    grid.gen_count = 0;

    // Set lock values to 0 (unlocked)
    grid.write_lock = 0;
    grid.read_lock = 0;

    // Allocate memory
    grid.data = malloc((width + 2) * sizeof(*grid.data));
    for (int i = 1; i < (width + 1); i++)
    {
        grid.data[i] = malloc((height + 2) * sizeof(*(grid.data[i])));
    }

    grid.data_prev = malloc((width + 2) * sizeof(*grid.data_prev));
    for (int i = 1; i < (width + 1); i++)
    {
        grid.data_prev[i] = malloc((height + 2) * sizeof(*(grid.data_prev[i])));
    }

    if (cyclic)
    {
        grid.data[0] = grid.data[width];
        grid.data[width + 1] = grid.data[1];

        grid.data_prev[0] = grid.data_prev[width];
        grid.data_prev[width + 1] = grid.data_prev[1];
    }
    else
    {
        grid.data[0] = malloc((height + 2) * sizeof(*(grid.data[0])));
        grid.data[width + 1] = malloc((height + 2) * sizeof(*(grid.data[0])));

        grid.data_prev[0] = malloc((height + 2) * sizeof(*(grid.data[0])));
        grid.data_prev[width + 1] = malloc((height + 2) * sizeof(*(grid.data[0])));
    }

    grid_clear(&grid);

    return grid;
}

// Free memory of grid data.
void free_grid_memory(Grid *grid)
{
    for (int i = 0; i < grid->width; i++)
    {
        free(grid->data[i]);
    }
    free(grid->data);
}

void grid_step(Grid *grid)
{
    switch (grid->algorithm)
    {
    case BASIC:
        grid_step_basic(grid);
        break;
    case BASIC_DIFF:
        grid_step_basic_diff(grid);
        break;
    case NEIGHBOURS:
        grid_step_neighbours(grid);
        break;
    case NEIGHBOURS_DIFF:
        grid_step_neighbours_diff(grid);
        break;
    }
}

// Set whole grid to empty cells.
void grid_clear(Grid *grid)
{
    SDL_AtomicLock(&grid->write_lock);

    for (int x = 0; x < (grid->width + 2); x++)
    {
        for (int y = 0; y < (grid->height + 2); y++)
        {
            grid->data[x][y] = 0;
            grid->data_prev[x][y] = 0;
        }
    }

    SDL_AtomicUnlock(&grid->write_lock);
}
