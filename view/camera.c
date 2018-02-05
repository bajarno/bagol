#include "camera.h"

#include <math.h>

// Allocate a new camera and return a pointer to it.
Camera *camera_init(uint64_t x, uint64_t y, uint64_t width, uint64_t height)
{
    Camera *camera = malloc(sizeof(Camera));

    camera->x = x;
    camera->y = y;

    camera->width = width;
    camera->height = height;

    return camera;
}

// Check if the camera overlaps the given quad. Returns boolean.
int camera_quad_overlap_check(Camera *camera, Quad *quad)
{
    uint64_t x = quad->x;
    x *= 6; // Multiplication on separate line because of uint32_t to uint64_t conversion.

    uint64_t y = quad->y;
    y *= 6; // Multiplication on separate line because of uint32_t to uint64_t conversion.

    // Width and height are equal to the amount of leafs on one side times the size
    // of the leaf on one side, which is 6 pixels.
    uint64_t size = pow(2, quad->level) * 6;

    return camera_overlap_check(camera, x, y, size, size);
}

// Check if the camera overlaps the given leaf. Returns boolean.
int camera_leaf_overlap_check(Camera *camera, Leaf *leaf)
{
    uint64_t x = leaf->x;
    x *= 6; // Multiplication on separate line because of uint32_t to uint64_t conversion.

    uint64_t y = leaf->y;
    y *= 6; // Multiplication on separate line because of uint32_t to uint64_t conversion.

    // Middle 6x6 cells are for visualisation.
    uint64_t size = 6;

    return camera_overlap_check(camera, x, y, size, size);
}

// Check if the camera overlaps the given rectangle properties. Returns boolean.
int camera_overlap_check(Camera *camera, uint64_t x, uint64_t y, uint64_t width, uint64_t height)
{
    return camera->x < x + width &&
           camera->x + camera->width > x &&
           camera->y < y + height &&
           camera->y + camera->height > y;
}