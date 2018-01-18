#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int width;
    int height;
    char ** data;
} grid;

void print_grid(grid grid) {
    for (int x = 0; x < grid.width; x++) {
        for (int y = 0; y < grid.height; y++) {
            printf("%d", grid.data[x][y]);
        }
        printf("\n");
    }
}

grid step_grid(grid grid) {

    char ** new_data = (char **)malloc(sizeof(char *) * grid.width);
    new_data[0] = (char *)malloc(sizeof(char) * grid.height * grid.width);

    for(int i = 0; i < grid.width; i++) {
        new_data[i] = (*new_data + grid.height * i);
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
                new_data[x][y] = grid.data[x][y];
            }
        }
    }

    grid.data = new_data;

    return grid;
}

grid init_grid(int width, int height) {
    grid grid;
    grid.width = width;
    grid.height = height;
    grid.data = (char **)malloc(sizeof(char *) * width);
    grid.data[0] = (char *)malloc(sizeof(char) * height * width);

    for(int i = 0; i < width; i++) {
        grid.data[i] = (*grid.data + height * i);
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            grid.data[x][y] = 0;
        }
    }

    return grid;
}

int main()
{
    int width = 20;
    int height = 20;

    grid grid = init_grid(width, height);

    // Add glider
    grid.data[2][1] = 1;
    grid.data[3][2] = 1;
    grid.data[1][3] = 1;
    grid.data[2][3] = 1;
    grid.data[3][3] = 1;

    // Add blinker
    grid.data[1][17] = 1;
    grid.data[2][17] = 1;
    grid.data[3][17] = 1;

    print_grid(grid);

    for (int i = 0; i < 100; i++) {
        // getchar();
        grid = step_grid(grid);
        printf("After step %d:\n",i);
        print_grid(grid);
    }

    return 0;
}