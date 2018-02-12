#include "tree_step.h"

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

        if (quad->metadata & (METADATA_EXIST_0 << i))
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
        if ((new_data & INTERNAL_N_MASK) != (old_data & INTERNAL_N_MASK))
        {
            Leaf *neighbour_north = tree_get_leaf(tree, x, y - 1);
            Block mask = (INTERNAL_N_MASK << 48) ^ -1;
            Block data = (new_data & INTERNAL_N_MASK) << 48;

            leaf_mask(neighbour_north, new_gen, mask, data);

            // data northwest corner changed
            if ((new_data & INTERNAL_NW_MASK) != (old_data & INTERNAL_NW_MASK))
            {
                Leaf *neighbour_north_west = tree_get_leaf(tree, x - 1, y - 1);
                Block mask = (INTERNAL_NW_MASK << 54) ^ -1;
                Block data = (new_data & INTERNAL_NW_MASK) << 54;

                leaf_mask(neighbour_north_west, new_gen, mask, data);
            }

            // data northeast corner changed
            if ((new_data & INTERNAL_NE_MASK) != (old_data & INTERNAL_NE_MASK))
            {
                Leaf *neighbour_north_east = tree_get_leaf(tree, x + 1, y - 1);
                Block mask = (INTERNAL_NE_MASK << 42) ^ -1;
                Block data = (new_data & INTERNAL_NE_MASK) << 42;

                leaf_mask(neighbour_north_east, new_gen, mask, data);
            }
        }

        // data east edge changed
        if ((new_data & INTERNAL_E_MASK) != (old_data & INTERNAL_E_MASK))
        {
            Leaf *neighbour_east = tree_get_leaf(tree, x + 1, y);
            Block mask = (INTERNAL_E_MASK >> 6) ^ -1;
            Block data = (new_data & INTERNAL_E_MASK) >> 6;

            leaf_mask(neighbour_east, new_gen, mask, data);
        }

        // data south edge changed
        if ((new_data & INTERNAL_S_MASK) != (old_data & INTERNAL_S_MASK))
        {
            Leaf *neighbour_south = tree_get_leaf(tree, x, y + 1);
            Block mask = (INTERNAL_S_MASK >> 48) ^ -1;
            Block data = (new_data & INTERNAL_S_MASK) >> 48;

            leaf_mask(neighbour_south, new_gen, mask, data);

            // data southwest corner changed
            if ((new_data & INTERNAL_SW_MASK) != (old_data & INTERNAL_SW_MASK))
            {
                Leaf *neighbour_south_west = tree_get_leaf(tree, x - 1, y + 1);
                Block mask = (INTERNAL_SW_MASK >> 42) ^ -1;
                Block data = (new_data & INTERNAL_SW_MASK) >> 42;

                leaf_mask(neighbour_south_west, new_gen, mask, data);
            }
            // data southeast corner changed
            if ((new_data & INTERNAL_SE_MASK) != (old_data & INTERNAL_SE_MASK))
            {
                Leaf *neighbour_south_east = tree_get_leaf(tree, x + 1, y + 1);
                Block mask = (INTERNAL_SE_MASK >> 54) ^ -1;
                Block data = (new_data & INTERNAL_SE_MASK) >> 54;

                leaf_mask(neighbour_south_east, new_gen, mask, data);
            }
        }

        // data west edge changed
        if ((new_data & INTERNAL_W_MASK) != (old_data & INTERNAL_W_MASK))
        {
            Leaf *neighbour_west = tree_get_leaf(tree, x - 1, y);
            Block mask = (INTERNAL_W_MASK << 6) ^ -1;
            Block data = (new_data & INTERNAL_W_MASK) << 6;

            leaf_mask(neighbour_west, new_gen, mask, data);
        }
    }
    // If the new data and the current data is both zero, the leaf can be
    // deleted from the tree to save computations.
    else if (!new_data && !data)
    {
        // Lock for rendering if structure changes
        SDL_AtomicLock(&tree->read_lock);
        tree_delete_leaf(leaf);
        SDL_AtomicUnlock(&tree->read_lock);
    }
}