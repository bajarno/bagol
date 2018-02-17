
#include "quad.h"

#include "leaf.c"

Quad *quad_init(uint32_t x, uint32_t y, uint8_t level, Quad *parent)
{
    Quad *quad = malloc(sizeof(Quad));

    quad->level = level;
    quad->x = x;
    quad->y = y;
    quad->metadata = 0;
    quad->parent = parent;

    // Allocate memory for pointers to sublayer
    quad->sub_quads = malloc(4 * sizeof(*quad->sub_quads));

    return quad;
}

void quad_set_check(Quad *quad, int value, int position)
{
    uint8_t old_metadata = quad->metadata;

    if (value)
    {
        quad->metadata |= metadata_check_mask[position];
    }
    else
    {
        quad->metadata &= metadata_check_unmask[position];
    }

    if (quad->metadata != old_metadata && quad->parent != NULL)
    {
        int check = (quad->metadata & metadata_check_mask_all) > 0;
        int position_in_parent = quad_global_to_local_pos(quad);

        quad_set_check(quad->parent, check, position_in_parent);
    }
}

int quad_get_check(Quad *quad)
{
    // If the quad has no parent, it is always checked.
    if (quad->parent == NULL)
    {
        return 1;
    }

    uint8_t metadata = quad->parent->metadata;

    int position = quad_global_to_local_pos(quad);
    uint8_t mask = metadata_check_mask[position];

    return (metadata & mask) > 0;
}

void quad_set_sub_quad(Quad *parent_quad, void *sub_quad, int position)
{
    parent_quad->sub_quads[position] = sub_quad;
    parent_quad->metadata |= metadata_exist_mask[position];
}

int quad_global_to_local_pos(Quad *quad)
{
    return global_to_local_pos(quad->x, quad->y, quad->level);
}

int global_to_local_pos(uint32_t x, uint32_t y, uint8_t level)
{
    return ((x & bit_mask[level]) > 0) + ((y & bit_mask[level]) > 0) * 2;
}

void quad_deinit(Quad *quad)
{
    free(quad->sub_quads);
    free(quad);
}