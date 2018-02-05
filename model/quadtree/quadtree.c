#include "quad.c"

#include "quadtree.h"

QuadTree *tree_init()
{
    QuadTree *tree = malloc(sizeof(QuadTree));

    tree->current_gen = 0;

    tree->parent_quad = quad_init(INITIAL_QUAD_POSITION, INITIAL_QUAD_POSITION, 1);

    // Set lock values to 0 (unlocked)
    tree->write_lock = 0;
    tree->read_lock = 0;

    return tree;
}

void tree_step(QuadTree *tree)
{
    SDL_AtomicLock(&tree->write_lock);

    tree_step_quad(tree, tree->parent_quad);

    SDL_AtomicLock(&tree->read_lock);
    tree->current_gen = !tree->current_gen;
    SDL_AtomicUnlock(&tree->read_lock);

    SDL_AtomicUnlock(&tree->write_lock);
}

void tree_step_quad(QuadTree *tree, Quad *quad)
{
    for (int i = 0; i < 4; i++)
    {
        if (quad->sub_quads[i] != NULL)
        {
            if (quad->level > 1)
            {
                tree_step_quad(tree, quad->sub_quads[i]);
            }
            else
            {
                tree_step_leaf(tree, quad->sub_quads[i]);
            }
        }
    }
}

void tree_step_leaf(QuadTree *tree, Leaf *leaf)
{
    int new_gen = !tree->current_gen;
    Block old_data = leaf->data[new_gen];

    leaf_step(leaf, tree->current_gen);

    Block data = leaf->data[tree->current_gen];
    Block new_data = leaf->data[new_gen];

    // If any data in the edges changed, update neighbouring blocks
    if ((new_data & INTERNAL_EDGE_MASK) != (old_data & INTERNAL_EDGE_MASK))
    {
        uint32_t x = leaf->x;
        uint32_t y = leaf->y;

        // data north edge changed
        if ((new_data & INTERNAL_EDGE_N_MASK) != (old_data & INTERNAL_EDGE_N_MASK))
        {
            Leaf *neighbour_north = tree_get_leaf(tree, x, y - 1);
            Block mask = (INTERNAL_EDGE_N_MASK << 48) ^ -1;
            Block data = (new_data & INTERNAL_EDGE_N_MASK) << 48;

            leaf_mask(neighbour_north, new_gen, mask, data);

            // data northwest corner changed
            if ((new_data & INTERNAL_EDGE_NW_MASK) != (old_data & INTERNAL_EDGE_NW_MASK))
            {
                Leaf *neighbour_north_west = tree_get_leaf(tree, x - 1, y - 1);
                Block mask = (INTERNAL_EDGE_NW_MASK << 54) ^ -1;
                Block data = (new_data & INTERNAL_EDGE_NW_MASK) << 54;

                leaf_mask(neighbour_north_west, new_gen, mask, data);
            }

            // data northeast corner changed
            if ((new_data & INTERNAL_EDGE_NE_MASK) != (old_data & INTERNAL_EDGE_NE_MASK))
            {
                Leaf *neighbour_north_east = tree_get_leaf(tree, x + 1, y - 1);
                Block mask = (INTERNAL_EDGE_NE_MASK << 42) ^ -1;
                Block data = (new_data & INTERNAL_EDGE_NE_MASK) << 42;

                leaf_mask(neighbour_north_east, new_gen, mask, data);
            }
        }

        // data east edge changed
        if ((new_data & INTERNAL_EDGE_E_MASK) != (old_data & INTERNAL_EDGE_E_MASK))
        {
            Leaf *neighbour_east = tree_get_leaf(tree, x + 1, y);
            Block mask = (INTERNAL_EDGE_E_MASK >> 6) ^ -1;
            Block data = (new_data & INTERNAL_EDGE_E_MASK) >> 6;

            leaf_mask(neighbour_east, new_gen, mask, data);
        }

        // data south edge changed
        if ((new_data & INTERNAL_EDGE_S_MASK) != (old_data & INTERNAL_EDGE_S_MASK))
        {
            Leaf *neighbour_south = tree_get_leaf(tree, x, y + 1);
            Block mask = (INTERNAL_EDGE_S_MASK >> 48) ^ -1;
            Block data = (new_data & INTERNAL_EDGE_S_MASK) >> 48;

            leaf_mask(neighbour_south, new_gen, mask, data);

            // data southwest corner changed
            if ((new_data & INTERNAL_EDGE_SW_MASK) != (old_data & INTERNAL_EDGE_SW_MASK))
            {
                Leaf *neighbour_south_west = tree_get_leaf(tree, x - 1, y + 1);
                Block mask = (INTERNAL_EDGE_SW_MASK >> 42) ^ -1;
                Block data = (new_data & INTERNAL_EDGE_SW_MASK) >> 42;

                leaf_mask(neighbour_south_west, new_gen, mask, data);
            }
            // data southeast corner changed
            if ((new_data & INTERNAL_EDGE_SE_MASK) != (old_data & INTERNAL_EDGE_SE_MASK))
            {
                Leaf *neighbour_south_east = tree_get_leaf(tree, x + 1, y + 1);
                Block mask = (INTERNAL_EDGE_SE_MASK >> 54) ^ -1;
                Block data = (new_data & INTERNAL_EDGE_SE_MASK) >> 54;

                leaf_mask(neighbour_south_east, new_gen, mask, data);
            }
        }

        // data west edge changed
        if ((new_data & INTERNAL_EDGE_W_MASK) != (old_data & INTERNAL_EDGE_W_MASK))
        {
            Leaf *neighbour_west = tree_get_leaf(tree, x - 1, y);
            Block mask = (INTERNAL_EDGE_W_MASK << 6) ^ -1;
            Block data = (new_data & INTERNAL_EDGE_W_MASK) << 6;

            leaf_mask(neighbour_west, new_gen, mask, data);
        }
    }
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

        Quad *new_parent_quad = quad_init(parent_quad->x & position_mask, parent_quad->y & position_mask, parent_level);
        new_parent_quad->sub_quads[old_parent_position] = parent_quad;

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
            quad->sub_quads[position] = quad_init(x & position_mask, y & position_mask, level);
        }

        quad = quad->sub_quads[position];
    }

    int position = global_to_local_pos(x, y, 0);

    if (quad->sub_quads[position] == NULL)
    {
        quad->sub_quads[position] = leaf_init(x, y);
    }

    return quad->sub_quads[position];
}

// Print a block to console in bit form.
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
