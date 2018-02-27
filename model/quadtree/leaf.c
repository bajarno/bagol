#include "block.c"

#include "leaf.h"

Leaf *leaf_init(uint32_t x, uint32_t y, Quad *parent)
{
    Leaf *leaf = malloc(sizeof(Leaf));

    leaf->x = x;
    leaf->y = y;

    leaf->data = calloc(2, sizeof(*leaf->data));
    leaf->data_change = 0;

    leaf->parent = parent;

    leaf->pos_in_parent = global_to_local_pos(x, y, 0);

    // Set lock values to 0 (unlocked)
    leaf->data_lock = 0;

    return leaf;
}

void leaf_step(Leaf *leaf, int base_gen)
{
    Block new_data = block_step(leaf->data[base_gen]);

    Block old_data = leaf->data[!base_gen];

    leaf->data[!base_gen] &= EXTERNAL_MASK;
    leaf->data[!base_gen] |= (INTERNAL_MASK & new_data);

    // If data changed, set checkbit for this leaf to true.
    if (leaf->data[!base_gen] != old_data)
    {
        leaf->data_change = 1;
        quad_set_check(leaf->parent, 1, leaf->pos_in_parent);
    }
}

void leaf_mask(Leaf *leaf, int gen, Block mask, Block data)
{
    SDL_AtomicLock(&leaf->data_lock);

    Block old_data = leaf->data[gen];

    leaf->data[gen] &= mask;
    leaf->data[gen] |= data;

    // If data changed, set checkbit for this leaf to true.
    if (leaf->data[gen] != old_data)
    {
        leaf->data_change = 1;
        quad_set_check(leaf->parent, 1, leaf->pos_in_parent);
    }

    SDL_AtomicUnlock(&leaf->data_lock);
}

int leaf_get_check(Leaf *leaf)
{
    uint8_t metadata = leaf->parent->metadata;

    uint8_t mask = metadata_check_mask[leaf->pos_in_parent];

    return (metadata & mask) > 0;
}

void leaf_deinit(Leaf *leaf)
{
    free(leaf->data);
    free(leaf);
}

uint8_t global_to_local_pos(uint32_t x, uint32_t y, uint8_t level)
{
    return ((x & bit_mask[level]) > 0) + ((y & bit_mask[level]) > 0) * 2;
}
