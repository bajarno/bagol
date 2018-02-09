#include "quad.c"

#include "quadtree.h"

#include "tree_step.c"

QuadTree *tree_init()
{
    QuadTree *tree = malloc(sizeof(QuadTree));

    tree->current_gen = 0;

    tree->parent_quad = quad_init(INITIAL_QUAD_POSITION, INITIAL_QUAD_POSITION, 1, NULL);

    // Set lock values to 0 (unlocked)
    tree->write_lock = 0;
    tree->read_lock = 0;

    return tree;
}

void leaf_delete(Leaf *leaf)
{
    int pos_in_parent = leaf_global_to_local_pos(leaf);
    Quad *parent = leaf->parent;
    free(parent->sub_quads[pos_in_parent]);
    parent->sub_quads[pos_in_parent] = NULL;

    quad_delete(parent);
}

void quad_delete(Quad *quad)
{
    // If the quad has no parent, it must not be deleted.
    if (quad->parent == NULL)
    {
        return;
    }

    // If the quad has any sub quad, it must not be deleted.
    for (int i = 0; i < 4; i++)
    {
        if (quad->sub_quads[i] != NULL)
        {
            return;
        }
    }

    int pos_in_parent = quad_global_to_local_pos(quad);
    Quad *parent = quad->parent;
    free(parent->sub_quads[pos_in_parent]);
    parent->sub_quads[pos_in_parent] = NULL;

    // TODO: FIX
    quad_delete(parent);
}

Leaf *tree_get_leaf(QuadTree *tree, uint32_t x, uint32_t y)
{
    Quad *parent_quad = tree->parent_quad;
    uint8_t parent_level = parent_quad->level;

    // Mask for checking position while ignoring lower levels.
    uint32_t position_mask = UINT32_MAX << parent_level;
    // Check if position is below the current parent quad, if not, expand tree at top.
    while ((parent_quad->x & position_mask) != (x & position_mask) || (parent_quad->y & position_mask) != (y & position_mask))
    {
        parent_level += 1;
        position_mask = UINT32_MAX << parent_level;

        int old_parent_position = quad_global_to_local_pos(parent_quad);

        Quad *new_parent_quad = quad_init(parent_quad->x & position_mask, parent_quad->y & position_mask, parent_level, NULL);
        new_parent_quad->sub_quads[old_parent_position] = parent_quad;

        parent_quad->parent = new_parent_quad;
        parent_quad = new_parent_quad;

        tree->parent_quad = parent_quad;
    }

    Quad *quad = parent_quad;
    uint8_t level = parent_level;
    // Walk down the tree till the lowest level, if a needed sub quad does not exist, it is created.
    while (level > 1)
    {
        level -= 1;
        position_mask = UINT32_MAX << level;

        int position = global_to_local_pos(x, y, level);

        // If the sub quad does not exist, create it
        if (quad->sub_quads[position] == NULL)
        {
            quad->sub_quads[position] = quad_init(x & position_mask, y & position_mask, level, quad);
        }

        quad = quad->sub_quads[position];
    }

    int position = global_to_local_pos(x, y, 0);
    // If the leaf does not exist, create it.
    if (quad->sub_quads[position] == NULL)
    {
        quad->sub_quads[position] = leaf_init(x, y, quad);
    }

    return quad->sub_quads[position];
}

void tree_set_leaf(QuadTree *tree, uint32_t x, uint32_t y, Block data)
{
    Leaf *leaf = tree_get_leaf(tree, x, y);

    Block mask = INTERNAL_MASK ^ -1;
    data = data & INTERNAL_MASK;
    leaf_mask(leaf, tree->current_gen, mask, data);

    // data north edge changed
    Leaf *neighbour_north = tree_get_leaf(tree, x, y - 1);
    mask = (INTERNAL_N_MASK << 48) ^ -1;
    Block data_n = (data & INTERNAL_N_MASK) << 48;

    leaf_mask(neighbour_north, tree->current_gen, mask, data_n);

    // data northwest corner changed
    Leaf *neighbour_north_west = tree_get_leaf(tree, x - 1, y - 1);
    mask = (INTERNAL_NW_MASK << 54) ^ -1;
    Block data_nw = (data & INTERNAL_NW_MASK) << 54;

    leaf_mask(neighbour_north_west, tree->current_gen, mask, data_nw);

    // data northeast corner changed
    Leaf *neighbour_north_east = tree_get_leaf(tree, x + 1, y - 1);
    mask = (INTERNAL_NE_MASK << 42) ^ -1;
    Block data_ne = (data & INTERNAL_NE_MASK) << 42;

    leaf_mask(neighbour_north_east, tree->current_gen, mask, data_ne);

    // data east edge changed
    Leaf *neighbour_east = tree_get_leaf(tree, x + 1, y);
    mask = (INTERNAL_E_MASK >> 6) ^ -1;
    Block data_e = (data & INTERNAL_E_MASK) >> 6;

    leaf_mask(neighbour_east, tree->current_gen, mask, data_e);

    // data south edge changed
    Leaf *neighbour_south = tree_get_leaf(tree, x, y + 1);
    mask = (INTERNAL_S_MASK >> 48) ^ -1;
    Block data_s = (data & INTERNAL_S_MASK) >> 48;

    leaf_mask(neighbour_south, tree->current_gen, mask, data_s);

    // data southwest corner changed
    Leaf *neighbour_south_west = tree_get_leaf(tree, x - 1, y + 1);
    mask = (INTERNAL_SW_MASK >> 42) ^ -1;
    Block data_sw = (data & INTERNAL_SW_MASK) >> 42;

    leaf_mask(neighbour_south_west, tree->current_gen, mask, data_sw);

    // data southeast corner changed
    Leaf *neighbour_south_east = tree_get_leaf(tree, x + 1, y + 1);
    mask = (INTERNAL_SE_MASK >> 54) ^ -1;
    Block data_se = (data & INTERNAL_SE_MASK) >> 54;

    leaf_mask(neighbour_south_east, tree->current_gen, mask, data_se);

    // data west edge changed
    Leaf *neighbour_west = tree_get_leaf(tree, x - 1, y);
    mask = (INTERNAL_W_MASK << 6) ^ -1;
    Block data_w = (data & INTERNAL_W_MASK) << 6;

    leaf_mask(neighbour_west, tree->current_gen, mask, data_w);
}

void tree_print_bits(QuadTree *tree)
{
    Block mask = 1;
    for (uint32_t leaf_y = INITIAL_QUAD_POSITION - 2; leaf_y < INITIAL_QUAD_POSITION + 3; leaf_y++)
    {
        for (int block_y = 0; block_y < 8; block_y++)
        {
            for (uint32_t leaf_x = INITIAL_QUAD_POSITION - 2; leaf_x < INITIAL_QUAD_POSITION + 3; leaf_x++)
            {
                for (int block_x = 0; block_x < 8; block_x++)
                {
                    Block mask = 1;
                    mask <<= (block_y * 8 + block_x);

                    Leaf *leaf = tree_get_leaf(tree, leaf_x, leaf_y);
                    Block data = leaf->data[tree->current_gen];
                    printf("%d", (data & mask) > 0);
                }
                printf(" ");
            }
            printf("\n");
        }
        printf("\n");
    }
}
