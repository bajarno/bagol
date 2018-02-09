#include "pattern.h"

#include "pattern_parse_rle.c"

Pattern *pattern_init(char *name, int width, int height)
{
    Pattern *pattern = malloc(sizeof(Pattern));

    pattern->name = strdup(name);

    pattern->width = width;
    pattern->height = height;

    // Allocate memory
    pattern->data = calloc(width, sizeof(*pattern->data));
    for (int i = 0; i < width; i++)
    {
        pattern->data[i] = calloc(height, sizeof(*(pattern->data[i])));
    }

    return pattern;
}

void tree_place_pattern(QuadTree *tree, Pattern *pattern, uint64_t x, uint64_t y)
{
    SDL_AtomicLock(&tree->write_lock);
    SDL_AtomicLock(&tree->read_lock);

    uint32_t leaf_min_x = x / 6;
    uint32_t leaf_min_y = y / 6;

    uint8_t leaf_offset_x = x % 6;
    uint8_t leaf_offset_y = y % 6;

    printf("x_offset: %d, y_offset: %d\n", leaf_offset_y, leaf_offset_y);

    uint32_t width_leafs = (pattern->width + leaf_offset_x + 5) / 6;
    uint32_t height_leafs = (pattern->height + leaf_offset_y + 5) / 6;

    for (uint32_t leaf_x = 0; leaf_x < width_leafs; leaf_x++)
    {
        for (uint32_t leaf_y = 0; leaf_y < height_leafs; leaf_y++)
        {
            // Retrieve leaf that should be edited.
            Leaf *leaf = tree_get_leaf(tree, leaf_x + leaf_min_x, leaf_y + leaf_min_y);
            Block data = leaf->data[tree->current_gen];

            if (leaf->x < 1000 || leaf->y < 1000)
            {
                printf("a\n");
            }

            // Loop over internal data of the leaf and edit according to the
            // pattern.
            for (int block_x = 1; block_x < 7; block_x++)
            {
                for (int block_y = 1; block_y < 7; block_y++)
                {
                    uint64_t cell_x = leaf_x * 6 + block_x - 1 - leaf_offset_x;
                    uint64_t cell_y = leaf_y * 6 + block_y - 1 - leaf_offset_y;

                    // If the cell is outside of the pattern boundaries, it is
                    // kept unchanged.
                    if (cell_x < 0 || cell_x >= pattern->width || cell_y < 0 || cell_y >= pattern->height)
                    {
                        continue;
                    }

                    // If the cell is alive in the pattern, it will be alive.
                    if (pattern->data[cell_x][cell_y])
                    {
                        Block mask = 1;
                        mask <<= (block_x + 8 * block_y);
                        data |= mask;
                    }
                }
            }

            tree_set_leaf(tree, leaf_x + leaf_min_x, leaf_y + leaf_min_y, data);
        }
    }

    SDL_AtomicUnlock(&tree->read_lock);
    SDL_AtomicUnlock(&tree->write_lock);
}

void pattern_print(Pattern *pattern)
{
    for (int y = 0; y < pattern->height; y++)
    {
        for (int x = 0; x < pattern->width; x++)
        {
            char c = pattern->data[x][y] ? 'o' : '-';
            printf("%c", c);
        }
        printf("\n");
    }
}

void pattern_deinit(Pattern *pattern)
{
    for (int i = 0; i < pattern->width; i++)
    {
        free(pattern->data[i]);
    }
    free(pattern->data);

    free(pattern);
}