#include "leaf.c"

#include "quad.h"

Quad * quad_init(uint32_t x, uint32_t y, uint8_t level) {
    Quad * quad = malloc(sizeof(Quad));

    quad->level = level;
    quad->x = x;
    quad->y = y;
    quad->metadata = 0;

    // Allocate memory for pointers to sublayer
    quad->sub_quads = calloc(4, sizeof(*quad->sub_quads));

    return quad;
}

// Print a block to console in bit form.
void quad_print_bits(Quad * quad, int gen) {
    Block mask = 1;
    for (int leaf_y = 0; leaf_y < 3; leaf_y++) {
        for (int block_y = 0; block_y < 8; block_y++) {
            for (int leaf_x = 0; leaf_x < 3; leaf_x++) {
                for (int block_x = 0; block_x < 8; block_x++) {
                    Block mask = 1;
                    mask <<= (block_y * 8 + block_x);

                    Leaf * leaf = quad_get_leaf(quad, leaf_x, leaf_y);
                    Block data = leaf->data[gen];
                    printf("%d", (data & mask) > 0);
                }
                printf(" ");
            }
            printf("\n");
        }
        printf("\n");
    }
}

void quad_step(Quad * quad, int base_gen) {
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            quad_step_leaf(quad, x, y, base_gen);
        }
    }
}

Leaf * quad_get_leaf(Quad * quad, uint32_t x, uint32_t y) {
    return (Leaf*)(quad->sub_quads[(x+3)%3]);
}

void quad_step_leaf(Quad * quad, uint32_t x, uint32_t y, int base_gen) {
    Leaf * leaf = quad_get_leaf(quad, x, y);

    int new_gen = !base_gen;
    Block old_data = leaf->data[new_gen];

    leaf_step(leaf, base_gen);


    Block data = leaf->data[base_gen];
    Block new_data = leaf->data[new_gen];

    // If any data in the edges changed, update neighbouring blocks
    if ((new_data & INTERNAL_EDGE_MASK) != (old_data & INTERNAL_EDGE_MASK)) {

        // data north edge changed
        if ((new_data & INTERNAL_EDGE_N_MASK) != (old_data & INTERNAL_EDGE_N_MASK)) {
            Leaf * neighbour_north = quad_get_leaf(quad, x, y-1);
            Block mask = (INTERNAL_EDGE_N_MASK << 48) ^ -1;
            Block data = (new_data & INTERNAL_EDGE_N_MASK) << 48;
            
            leaf_mask(neighbour_north, new_gen, mask, data);

            // data northwest corner changed
            if ((new_data & INTERNAL_EDGE_NW_MASK) != (old_data & INTERNAL_EDGE_NW_MASK)) {
                Leaf * neighbour_north_west = quad_get_leaf(quad, x-1, y-1);
                Block mask = (INTERNAL_EDGE_NW_MASK << 54) ^ -1;
                Block data = (new_data & INTERNAL_EDGE_NW_MASK) << 54;
                
                leaf_mask(neighbour_north_west, new_gen, mask, data);
            }

            // data northeast corner changed
            if ((new_data & INTERNAL_EDGE_NE_MASK) != (old_data & INTERNAL_EDGE_NE_MASK)) {
                Leaf * neighbour_north_east = quad_get_leaf(quad, x+1, y-1);
                Block mask = (INTERNAL_EDGE_NE_MASK << 42) ^ -1;
                Block data = (new_data & INTERNAL_EDGE_NE_MASK) << 42;
                
                leaf_mask(neighbour_north_east, new_gen, mask, data);
            }
        }

        // data east edge changed
        if ((new_data & INTERNAL_EDGE_E_MASK) != (old_data & INTERNAL_EDGE_E_MASK)) {
            Leaf * neighbour_east = quad_get_leaf(quad, x+1, y);
            Block mask = (INTERNAL_EDGE_E_MASK >> 6) ^ -1;
            Block data = (new_data & INTERNAL_EDGE_E_MASK) >> 6;
            
            leaf_mask(neighbour_east, new_gen, mask, data);
        }

        // data south edge changed
        if ((new_data & INTERNAL_EDGE_S_MASK) != (old_data & INTERNAL_EDGE_S_MASK)) {
            Leaf * neighbour_south = quad_get_leaf(quad, x, y+1);
            Block mask = (INTERNAL_EDGE_S_MASK >> 48) ^ -1;
            Block data = (new_data & INTERNAL_EDGE_S_MASK) >> 48;
            
            leaf_mask(neighbour_south, new_gen, mask, data);
            
            
            // data southwest corner changed
            if ((new_data & INTERNAL_EDGE_SW_MASK) != (old_data & INTERNAL_EDGE_SW_MASK)) {
                Leaf * neighbour_south_west = quad_get_leaf(quad, x-1, y+1);
                Block mask = (INTERNAL_EDGE_SW_MASK >> 42) ^ -1;
                Block data = (new_data & INTERNAL_EDGE_SW_MASK) >> 42;
                
                leaf_mask(neighbour_south_west, new_gen, mask, data);
            }
            // data southeast corner changed
            if ((new_data & INTERNAL_EDGE_SE_MASK) != (old_data & INTERNAL_EDGE_SE_MASK)) {
                Leaf * neighbour_south_east = quad_get_leaf(quad, x+1, y+1);
                Block mask = (INTERNAL_EDGE_SE_MASK >> 54) ^ -1;
                Block data = (new_data & INTERNAL_EDGE_SE_MASK) >> 54;
                
                leaf_mask(neighbour_south_east, new_gen, mask, data);
            }
        }

        // data west edge changed
        if ((new_data & INTERNAL_EDGE_W_MASK) != (old_data & INTERNAL_EDGE_W_MASK)) {
            Leaf * neighbour_west = quad_get_leaf(quad, x-1, y);
            Block mask = (INTERNAL_EDGE_W_MASK << 6) ^ -1;
            Block data = (new_data & INTERNAL_EDGE_W_MASK) << 6;
            
            leaf_mask(neighbour_west, new_gen, mask, data);
        }
    }
}

int quad_global_to_local_pos(Quad * quad) {
    return global_to_local_pos(quad->x, quad->y, quad->level);
}

int global_to_local_pos(uint32_t x, uint32_t y, uint8_t level) {
    uint32_t position_mask = 1 << level;

    return ((x & position_mask) > 0) + ((y & position_mask) > 0) * 2;
}