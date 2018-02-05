// Represents a node in a quadtree.
typedef struct Quad
{
    // Value indicating the level of the quad. Cell level is 0.
    uint8_t level;

    uint32_t x;
    uint32_t y;

    // Represents metadata of subquads. b
    uint8_t metadata;

    // Pointers to subquads. If level = 1, points to a block of cells. Otherwise, points to other quad instances.
    void **sub_quads;

    struct Quad *parent;
} Quad;

Quad *quad_init(uint32_t, uint32_t, uint8_t, Quad *);
int quad_global_to_local_pos(Quad *);
int global_to_local_pos(uint32_t, uint32_t, uint8_t);