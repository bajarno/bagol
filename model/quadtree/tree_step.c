#include "tree_step.h"

void tree_step(QuadTree *tree)
{
    SDL_AtomicLock(&tree->write_lock);

    QuadStepData data;
    data.tree = tree;
    data.quad = tree->parent_quad;
    tree_step_quad(&data);

    tree->current_gen = !tree->current_gen;

    SDL_AtomicUnlock(&tree->write_lock);
}

int tree_step_quad(void *void_data)
{
    QuadStepData *data = (QuadStepData *)(void_data);

    SDL_Thread **threads = calloc(4, sizeof(*threads));
    QuadStepData **step_data = calloc(4, sizeof(*step_data));

    for (int i = 0; i < 4; i++)
    {
        if (data->quad->metadata & metadata_check_mask[i])
        {
            if (data->quad->level > 1 && data->quad->level % 9 == 0)
            {
                step_data[i] = malloc(sizeof(step_data[i]));
                step_data[i]->tree = data->tree;
                step_data[i]->quad = data->quad->sub_quads[i];

                threads[i] = SDL_CreateThread(tree_step_quad, "Quad_Step_Thread", step_data[i]);
            }
            else if (data->quad->level > 1)
            {
                step_data[i] = malloc(sizeof(step_data[i]));
                step_data[i]->tree = data->tree;
                step_data[i]->quad = data->quad->sub_quads[i];

                tree_step_quad(step_data[i]);
            }
            else
            {
                tree_step_leaf(data->tree, data->quad->sub_quads[i]);
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        if (threads[i] != NULL)
        {
            SDL_WaitThread(threads[i], NULL);
        }
        free(step_data[i]);
    }

    free(threads);
    free(step_data);

    return 0;
}

void tree_step_leaf(QuadTree *tree, Leaf *leaf)
{
    SDL_AtomicLock(&leaf->data_lock);

    int new_gen = !tree->current_gen;
    Block old_data = leaf->data[new_gen];

    leaf_step(leaf, tree->current_gen);

    Block data = leaf->data[tree->current_gen];
    Block new_data = leaf->data[new_gen];

    // If the data did not change since the last step, it does not need to be
    // checked.
    if (!leaf->data_change)
    {
        quad_set_check(leaf->parent, 0, leaf->pos_in_parent);
    }

    uint32_t x = leaf->x;
    uint32_t y = leaf->y;

    // If the new data and the current data is both zero, the leaf can be
    // deleted from the tree to save computations.
    if (!new_data && !data)
    {
        tree_delete_leaf(leaf);
    }
    else
    {
        // Reset data_change to 0.
        leaf->data_change = 0;

        SDL_AtomicUnlock(&leaf->data_lock);
    }

    // If any data in the edges changed, update neighbouring blocks
    if ((new_data & INTERNAL_EDGE_MASK) != (old_data & INTERNAL_EDGE_MASK))
    {
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
}