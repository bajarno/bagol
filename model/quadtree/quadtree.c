#include "quad.c"

#include "quadtree.h"

QuadTree * tree_init() {
    QuadTree * tree = malloc(sizeof(QuadTree));

    tree->current_gen = 0;

    tree->parent_quad = quad_init(INITIAL_QUAD_POSITION, INITIAL_QUAD_POSITION, 1);

    return tree;
}


Leaf * tree_get_leaf(QuadTree * tree, uint32_t x, uint32_t y) {
    Quad * parent_quad = tree->parent_quad;
    uint8_t parent_level = parent_quad->level;

    // Mask for checking position while ignoring lower levels.
    uint32_t position_mask = UINT32_MAX << parent_level;
    // Check if position is below the current parent quad, if not, expand tree at top.
    while ((parent_quad->x & position_mask) != (x & position_mask) || (parent_quad->y & position_mask) != (y & position_mask)) {
        printf("Create parent quad\n");
        int old_parent_position = quad_global_to_local_pos(parent_quad);

        Quad * new_parent_quad = quad_init(parent_quad->x, parent_quad->y, parent_level + 1);
        new_parent_quad->sub_quads[old_parent_position] = parent_quad;

        parent_quad = new_parent_quad;
        parent_level += 1;
        position_mask = UINT32_MAX << parent_level;

        tree->parent_quad = parent_quad;
    }

    Quad * quad = parent_quad;
    uint8_t level = parent_level;
    // Walk down the tree till the lowest level, if a needed sub quad does not exist, it is created.
    while (level > 0) {
        int position = global_to_local_pos(x, y, level);

        // If the sub quad does not exist, create it
        if (quad->sub_quads[position] == NULL) {
            printf("Create child quad\n");
            quad->sub_quads[position] = quad_init(x, y, level - 1);
        } 

        quad = quad->sub_quads[position];
        level -= 1;
    }
    
    int position = global_to_local_pos(x, y, 0);

    if (quad->sub_quads[position] == NULL) {
        printf("Create leaf\n");
        quad->sub_quads[position] = leaf_init(x, y);
    }

    return quad->sub_quads[position];
}

