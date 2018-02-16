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

void tree_delete_leaf(Leaf *leaf)
{
    int pos_in_parent = leaf_global_to_local_pos(leaf);
    Quad *parent = leaf->parent;

    // Nonexistent leaf should not be checked.
    quad_set_check(parent, 0, pos_in_parent);

    leaf_deinit(leaf);
    parent->sub_quads[pos_in_parent] = NULL;
    parent->metadata &= (METADATA_EXIST_0 << pos_in_parent) ^ -1;

    tree_delete_quad(parent);
}

void tree_delete_quad(Quad *quad)
{
    // If the quad has no parent, it must not be deleted.
    if (quad->parent == NULL)
    {
        return;
    }

    // If the quad has any sub quad, it must not be deleted.
    for (int i = 0; i < 4; i++)
    {
        if (quad->metadata & (METADATA_EXIST_0 << i))
        {
            return;
        }
    }

    int pos_in_parent = quad_global_to_local_pos(quad);
    Quad *parent = quad->parent;

    // Nonexistent quad should not be checked.
    quad_set_check(parent, 0, pos_in_parent);

    quad_deinit(quad);
    parent->sub_quads[pos_in_parent] = NULL;
    parent->metadata &= (METADATA_EXIST_0 << pos_in_parent) ^ -1;

    tree_delete_quad(parent);
}

Leaf *tree_get_leaf(QuadTree *tree, uint32_t x, uint32_t y)
{
    Quad *parent = tree->parent_quad;
    uint8_t parent_level = parent->level;

    // Mask for checking position while ignoring lower levels.
    uint32_t position_mask = UINT32_MAX << parent_level;
    // Check if position is below the current parent quad, if not, expand tree at top.
    while ((parent->x & position_mask) != (x & position_mask) || (parent->y & position_mask) != (y & position_mask))
    {
        parent_level += 1;
        position_mask = UINT32_MAX << parent_level;

        int old_parent_position = quad_global_to_local_pos(parent);

        Quad *new_parent = quad_init(parent->x & position_mask, parent->y & position_mask, parent_level, NULL);
        quad_set_sub_quad(new_parent, parent, old_parent_position);
        quad_set_check(new_parent, 1, old_parent_position);

        parent->parent = new_parent;
        parent = new_parent;

        tree->parent_quad = parent;
    }

    Quad *quad = parent;
    uint8_t level = parent_level;
    // Walk down the tree till the lowest level, if a needed sub quad does not exist, it is created.
    while (level > 1)
    {
        level -= 1;
        position_mask = UINT32_MAX << level;

        int position = global_to_local_pos(x, y, level);

        // If the sub quad does not exist, create it
        if (!(quad->metadata & (METADATA_EXIST_0 << position)))
        {
            Quad *sub_quad = quad_init(x & position_mask, y & position_mask, level, quad);
            quad_set_sub_quad(quad, sub_quad, position);
        }

        quad = quad->sub_quads[position];
    }

    int position = global_to_local_pos(x, y, 0);
    // If the leaf does not exist, create it.
    if (!(quad->metadata & (METADATA_EXIST_0 << position)))
    {
        Leaf *leaf = leaf_init(x, y, quad);
        quad_set_sub_quad(quad, leaf, position);

        // New leafs must always be checked since they otherwise can stay empty
        // without being removed from the tree.
        quad_set_check(leaf->parent, 1, position);
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
