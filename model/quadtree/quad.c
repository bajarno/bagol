
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

void quad_set_sub_quad(Quad *parent_quad, void *sub_quad, uint8_t position)
{
    parent_quad->sub_quads[position] = sub_quad;
    parent_quad->metadata |= (METADATA_EXIST_0 << position);
}

int quad_global_to_local_pos(Quad *quad)
{
    return global_to_local_pos(quad->x, quad->y, quad->level);
}

int global_to_local_pos(uint32_t x, uint32_t y, uint8_t level)
{
    uint32_t position_mask = 1 << level;

    return ((x & position_mask) > 0) + ((y & position_mask) > 0) * 2;
}

void quad_deinit(Quad *quad)
{
    free(quad->sub_quads);
    free(quad);
}