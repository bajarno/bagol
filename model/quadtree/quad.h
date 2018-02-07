// Represents a node in a quadtree.
typedef struct Quad
{
    // Value indicating the level of the quad. Leaf level is 0.
    uint8_t level;

    // Position.
    uint32_t x;
    uint32_t y;

    // Pointers to subquads. If level = 1, points to a leaf. Otherwise, points
    // to other quad instances.
    void **sub_quads;

    // The quad containing the quad, or NULL if quad is top layer.
    struct Quad *parent;
} Quad;

// Initializes a new quad instance.
Quad *quad_init(uint32_t, uint32_t, uint8_t, Quad *);

// Returns the position of the quad in the parent quad.
int quad_global_to_local_pos(Quad *);

// Returns the given position in the parent quad.
int global_to_local_pos(uint32_t, uint32_t, uint8_t);