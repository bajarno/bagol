
#include "quad.h"

#include "leaf.c"

Quad *quad_init(uint32_t x, uint32_t y, uint8_t level, Quad *parent)
{
    Quad *quad = malloc(sizeof(Quad));

    quad->level = level;
    quad->x = x;
    quad->y = y;
    quad->metadata = 0;

    // Allocate memory for pointers to sublayer
    quad->sub_quads = calloc(4, sizeof(*quad->sub_quads));

    quad->parent = parent;

    return quad;
}

int quad_global_to_local_pos(Quad *quad)
{
    return global_to_local_pos(quad->x, quad->y, quad->level);
}

int global_to_local_pos(uint32_t x, uint32_t y, uint8_t level)
{
    uint32_t position_mask = 1 << level;

    int xdif = ((x & position_mask) > 0);
    int ydif = ((y & position_mask) > 0) * 2;
    int sum = xdif + ydif;
    return sum;
}