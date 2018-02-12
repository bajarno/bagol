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
    int next_gen = !base_gen;

    Block new_data = block_step(leaf->data[base_gen]);

    leaf_mask(leaf, next_gen, INTERNAL_MASK ^ -1, INTERNAL_MASK & new_data);
}

void leaf_mask(Leaf *leaf, int gen, Block mask, Block data)
{
    leaf->data[gen] &= mask;
    leaf->data[gen] |= data;
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
