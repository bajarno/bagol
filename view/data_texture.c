#include "data_texture.h"

SDL_Texture *data_texture_init(SDL_Renderer *renderer, uint64_t width, uint64_t height)
{
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA4444, SDL_TEXTUREACCESS_STATIC, width, height);

    return texture;
}

void data_texture_resize(RenderData *render_data, uint64_t width, uint64_t height)
{
    SDL_AtomicLock(&render_data->write_lock);

    render_data->camera->width = width;
    render_data->camera->height = height;
    SDL_DestroyTexture(render_data->data_texture);
    render_data->data_texture = data_texture_init(render_data->renderer, width, height);

    SDL_AtomicUnlock(&render_data->write_lock);
}

void data_texture_update_grid(RenderData *render_data, Grid *grid)
{
    SDL_AtomicLock(&render_data->write_lock);

    int width;
    int height;

    SDL_QueryTexture(render_data->data_texture, NULL, NULL, &width, &height);

    int size = width * height;
    uint16_t *pixels = malloc(size * sizeof(*pixels));

    SDL_AtomicLock(&grid->read_lock);
    for (int i = 0; i < grid->width * grid->height; i++)
    {
        if (render_data->debug_mode)
        {
            // Show colors indicating which cells are being checked
            uint16_t state = (grid->data[i % grid->width + 1][i / grid->width + 1] & STATEMASK) / STATEMASK;
            uint16_t check = (grid->data[i % grid->width + 1][i / grid->width + 1] & CHECKMASK) / CHECKMASK;
            uint16_t color = state * 61455 + check * 255;
            pixels[i] = color;
        }
        else
        {
            // Show plain live-death state.
            pixels[i] = (grid->data[i % grid->width + 1][i / grid->width + 1] & STATEMASK) ? 61167 : 4383;
        }
    }
    SDL_AtomicUnlock(&grid->read_lock);

    SDL_UpdateTexture(render_data->data_texture, NULL, pixels, width * sizeof(uint16_t));

    free(pixels);

    SDL_AtomicUnlock(&render_data->write_lock);
}

void data_texture_update_tree(RenderData *render_data, QuadTree *tree)
{
    SDL_AtomicLock(&render_data->write_lock);

    int width;
    int height;

    SDL_QueryTexture(render_data->data_texture, NULL, NULL, &width, &height);

    int size = width * height;
    uint16_t *pixels = calloc(size, sizeof(*pixels));

    data_texture_update_quad(pixels, render_data, tree, tree->parent_quad);

    SDL_UpdateTexture(render_data->data_texture, NULL, pixels, width * sizeof(uint16_t));

    free(pixels);

    SDL_AtomicUnlock(&render_data->write_lock);
}

void data_texture_update_quad(uint16_t *pixels, RenderData *render_data, QuadTree *tree, Quad *quad)
{
    SDL_AtomicLock(&quad->read_lock);

    if (camera_quad_overlap_check(render_data->camera, quad))
    {
        if (render_data->debug_mode)
        {
            int color = quad_get_check(quad) ? 254 - quad->level : 15 - quad->level;

            int min_x = quad->x * 6 - render_data->camera->x;
            min_x = min_x < 0 ? 0 : min_x;

            int max_x = (quad->x + pow(2, quad->level)) * 6 - render_data->camera->x;
            max_x = max_x > render_data->camera->width ? render_data->camera->width : max_x;

            int min_y = quad->y * 6 - render_data->camera->y;
            min_y = min_y < 0 ? 0 : min_y;

            int max_y = (quad->y + pow(2, quad->level)) * 6 - render_data->camera->y;
            max_y = max_y > render_data->camera->height ? render_data->camera->height : max_y;

            int width = (max_x - min_x) * 2;
            for (int y = min_y; y < max_y; y++)
            {
                memset(&pixels[min_x + y * render_data->camera->width], color, width);
            }
        }

        for (int i = 0; i < 4; i++)
        {
            if (quad->metadata & metadata_exist_mask[i])
            {
                if (quad->level > 1)
                {
                    data_texture_update_quad(pixels, render_data, tree, quad->sub_quads[i]);
                }
                else
                {
                    data_texture_update_leaf(pixels, render_data, tree, quad->sub_quads[i]);
                }
            }
        }
    }

    SDL_AtomicUnlock(&quad->read_lock);
}

void data_texture_update_leaf(uint16_t *pixels, RenderData *render_data, QuadTree *tree, Leaf *leaf)
{
    if (camera_leaf_overlap_check(render_data->camera, leaf))
    {
        // Depending on the position not all cells of the leaf need to be rendered.
        int min_block_x = render_data->camera->x - leaf->x * 6 + 1;
        min_block_x = min_block_x < 1 ? 1 : min_block_x;

        int max_block_x = render_data->camera->x + render_data->camera->width - leaf->x * 6 + 1;
        max_block_x = max_block_x > 7 ? 7 : max_block_x;

        int min_block_y = render_data->camera->y - leaf->y * 6 + 1;
        min_block_y = min_block_y < 1 ? 1 : min_block_y;

        int max_block_y = render_data->camera->y + render_data->camera->height - leaf->y * 6 + 1;
        max_block_y = max_block_y > 7 ? 7 : max_block_y;

        for (int block_x = min_block_x; block_x < max_block_x; block_x++)
        {
            for (int block_y = min_block_y; block_y < max_block_y; block_y++)
            {
                Block mask = 1;
                mask <<= (block_y * 8 + block_x);

                Block data = leaf->data[tree->current_gen];

                int state = (data & mask) > 0;
                uint32_t pos = leaf->x * 6 + block_x - 1 - render_data->camera->x + (leaf->y * 6 + block_y - 1 - render_data->camera->y) * render_data->camera->width;

                // TODO: fix such that this is not neccesary
                if (pos > render_data->camera->width * render_data->camera->height)
                {
                    continue;
                }

                if (render_data->debug_mode)
                {
                    pixels[pos] = state ? 0 : leaf_get_check(leaf) ? 65535 : 3855;
                }
                else
                {
                    pixels[pos] = state ? 61167 : 0;
                }
            }
        }
    }
}