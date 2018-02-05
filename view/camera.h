// Struct representing the camera.
typedef struct Camera
{
    uint64_t x;
    uint64_t y;
    uint64_t width;
    uint64_t height;
} Camera;

int camera_quad_overlap_check(Camera *, Quad *);
int camera_leaf_overlap_check(Camera *, Leaf *);
int camera_overlap_check(Camera *, uint64_t, uint64_t, uint64_t, uint64_t);