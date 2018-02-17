#include "block.c"

#include "leaf.h"

Leaf *leaf_init(uint32_t x, uint32_t y, Quad *parent)
{
    Leaf *leaf = malloc(sizeof(Leaf));

    leaf->x = x;
    leaf->y = y;

    leaf->data = calloc(2, sizeof(*leaf->data));

    leaf->parent = parent;

    return leaf;
}

void leaf_step(Leaf *leaf, int base_gen)
{
    Block new_data = block_step(leaf->data[base_gen]);

    leaf_mask(leaf, !base_gen, EXTERNAL_MASK, INTERNAL_MASK & new_data);
}

void leaf_mask(Leaf *leaf, int gen, Block mask, Block data)
{
    Block old_data = leaf->data[gen];

    leaf->data[gen] &= mask;
    leaf->data[gen] |= data;

    // If data changed, set checkbit for this leaf to true.
    if (leaf->data[gen] != old_data)
    {
        int position = leaf_global_to_local_pos(leaf);
        quad_set_check(leaf->parent, 1, position);
    }
}

int leaf_get_check(Leaf *leaf)
{
    uint8_t metadata = leaf->parent->metadata;

    int position = leaf_global_to_local_pos(leaf);
    uint8_t mask = metadata_check_mask[position];

    return (metadata & mask) > 0;
}

int leaf_global_to_local_pos(Leaf *leaf)
{
    return global_to_local_pos(leaf->x, leaf->y, 0);
}

void leaf_deinit(Leaf *leaf)
{
    free(leaf->data);
    free(leaf);
}
