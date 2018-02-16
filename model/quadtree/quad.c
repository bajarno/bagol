
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

void quad_set_check(Quad *quad, int value, int position)
{
    uint8_t old_metadata = quad->metadata;
    uint8_t mask = METADATA_CHECK_0 << position;

    if (value)
    {
        quad->metadata |= mask;
    }
    else
    {
        quad->metadata &= (mask ^ -1);
    }

    if (quad->metadata != old_metadata && quad->parent != NULL)
    {
        int check = (quad->metadata & METADATA_CHECK) > 0;
        int position_in_parent = quad_global_to_local_pos(quad);

        quad_set_check(quad->parent, check, position_in_parent);
    }
}

int quad_get_check(Quad *quad)
{
    if (quad->parent == NULL)
    {
        return 1;
    }

    uint8_t metadata = quad->parent->metadata;

    int position = quad_global_to_local_pos(quad);
    uint8_t mask = METADATA_CHECK_0 << position;

    return (metadata & mask) > 0;
}

void quad_set_sub_quad(Quad *parent_quad, void *sub_quad, int position)
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