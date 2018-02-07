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

// void tree_place_pattern(QuadTree *tree, Pattern *pattern, int32u_t x, int32u_t y)
// {
// }