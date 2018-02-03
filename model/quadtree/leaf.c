#include "block.c"

#include "leaf.h"

Leaf * leaf_init(uint32_t x, uint32_t y) {
    Leaf * leaf = malloc(sizeof(Leaf));

    leaf->x = x;
    leaf->y = y;

    leaf->data = calloc(2, sizeof(*leaf->data));

    return leaf;
}

void leaf_step(Leaf * leaf, int base_gen) {
    int next_gen = !base_gen;

    Block new_data = block_step(leaf->data[base_gen]);

    leaf->data[next_gen] &= (INTERNAL_MASK ^ -1);
    leaf->data[next_gen] |= (INTERNAL_MASK & new_data);
}

void leaf_mask(Leaf * leaf, int gen, Block mask, Block data) {    
    leaf->data[gen] &= mask;
    leaf->data[gen] |= data;
}
