#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "model/quadtree/quadtree.c"
#include "model/grid.c"
#include "model/pattern.c"
#include "view/graphics.c"

#include "main.h"

int main(int argc, char **argv)
{
    // Settings
    int screen_width = 600;
    int screen_height = 600;
    int grid_width = screen_width;
    int grid_height = screen_height;

    int data_structure = QUADTREE;

    // Setup data
    AppData data;

    // Graphics
    data.render_data = sdl_init(screen_width, screen_height);
    data.data_structure = data_structure;

    if (data_structure == GRID)
    {
        // Grid
        Grid grid = grid_init(grid_width, grid_height, 1, NEIGHBOURS_DIFF);
        data.grid = &grid;
    }
    else
    {
        data.tree = tree_init();
        Leaf *leaf = tree_get_leaf(data.tree, INITIAL_QUAD_POSITION, INITIAL_QUAD_POSITION);
        leaf->data[data.tree->current_gen] = 8848035282944;
    }

    // Initial values
    data.fps = 0;
    data.ups = 0;
    data.quit = 0;

    // Draw loop, run in separate thread.
    SDL_Thread *draw_thread = SDL_CreateThread(draw_loop, "DrawThread", &data);

    // Update loop, run in separate thread.
    SDL_Thread *update_thread = SDL_CreateThread(update_loop, "UpdateThread", &data);

    // Event loop, run in main thread.
    event_loop(&data);

    // Quit
    SDL_WaitThread(draw_thread, NULL);
    SDL_WaitThread(update_thread, NULL);
    sdl_quit(data.render_data);

    return 0;
}

int draw_loop(void *data)
{
    AppData *app_data = (AppData *)(data);

    Uint32 previous_ticks = 0;
    float frametime = 60;
    while (!app_data->quit)
    {
        // Calculate fps
        Uint32 ticks = SDL_GetTicks();
        Uint32 delta_ticks = ticks - previous_ticks;
        previous_ticks = ticks;

        // Calculate frametime by actual frametime and previous frametime for smoothing
        float smooth_factor = 0.1;
        frametime = smooth_factor * delta_ticks + (1.0 - smooth_factor) * frametime;
        app_data->fps = 1000.0 / frametime;

        // Update texture
        if (app_data->data_structure == GRID)
        {
            data_texture_update_grid(app_data->render_data, app_data->grid);
        }
        else
        {
            data_texture_update_tree(app_data->render_data, app_data->tree);
        }

        char debug_text_format[] = "FPS: %.1f UPS: %.1f";
        char debug_text[1000] = {};
        snprintf(debug_text, 999, debug_text_format, app_data->fps, app_data->ups);
        update_debug_texture(debug_text, app_data->render_data);

        render(app_data->render_data);
    }

    return 0;
}

int event_loop(void *data)
{
    AppData *app_data = (AppData *)(data);

    // Event loop
    SDL_Event event;
    while (SDL_WaitEvent(&event) && !app_data->quit)
    {
        if (event.type == SDL_QUIT)
        {
            app_data->quit = 1;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                app_data->quit = 1;
                break;
            case SDLK_SPACE:
                grid_clear(app_data->grid);
                break;
            case 'c':
                add_pattern(RPENTOMINO_SPAM, app_data->grid, 0, 0);
                break;
            case 'd':
                app_data->render_data->debug_mode = !app_data->render_data->debug_mode;
                break;
            case SDLK_UP:
                app_data->render_data->camera->y -= 10;
                break;
            case SDLK_RIGHT:
                app_data->render_data->camera->x += 10;
                break;
            case SDLK_DOWN:
                app_data->render_data->camera->y += 10;
                break;
            case SDLK_LEFT:
                app_data->render_data->camera->x -= 10;
                break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN && app_data->data_structure != QUADTREE)
        {
            int width;
            int height;
            SDL_GetWindowSize(app_data->render_data->window, &width, &height);

            int x = event.button.x * app_data->grid->width / width;
            int y = event.button.y * app_data->grid->height / height;
            add_pattern(RPENTOMINO, app_data->grid, x, y);
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            if (event.motion.state & SDL_BUTTON_LMASK)
            {
                float scale = app_data->render_data->camera->width / (float)app_data->render_data->window_width;
                app_data->render_data->camera->x -= (int)floor(event.motion.xrel * scale + 0.5);
                app_data->render_data->camera->y -= (int)floor(event.motion.yrel * scale + 0.5);
            }
        }
        else if (event.type == SDL_MOUSEWHEEL)
        {
            float zoom = 1 - event.wheel.y * 0.1;
            uint64_t width = app_data->render_data->camera->width * zoom;
            uint64_t height = app_data->render_data->camera->height * zoom;
            data_texture_resize(app_data->render_data, width, height);
        }
        else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            app_data->render_data->window_width = event.window.data1;
            app_data->render_data->window_width = event.window.data2;
            data_texture_resize(app_data->render_data, event.window.data1, event.window.data2);
        }
    }

    return 0;
}

int update_loop(void *data)
{
    AppData *app_data = (AppData *)(data);

    Uint32 previous_ticks = 0;
    float updatetime = 60;
    while (!app_data->quit)
    {
        if (app_data->data_structure == GRID)
        {
            // Generate new generation
            grid_step(app_data->grid);
            app_data->grid->gen_count++;
        }
        else
        {
            // SDL_Delay(100);
            tree_step(app_data->tree);
        }

        // Calculate ups
        Uint32 ticks = SDL_GetTicks();
        Uint32 delta_ticks = ticks - previous_ticks;
        previous_ticks = ticks;

        // Calculate updatetime by actual updatetime and previous updatetime for smoothing
        float smooth_factor = 0.01;
        updatetime = smooth_factor * delta_ticks + (1.0 - smooth_factor) * updatetime;
        app_data->ups = 1000.0 / updatetime;
    }

    return 0;
}
