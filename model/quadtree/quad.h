uint8_t METADATA_CHECK = 15;

uint8_t METADATA_CHECK_0 = 1;
uint8_t METADATA_EXIST_0 = 16;

// Represents a node in a quadtree.
typedef struct Quad
{
    // Value indicating the level of the quad. Leaf level is 0.
    uint8_t level;

    // Position.
    uint32_t x;
    uint32_t y;

    // 8 bits representing metadata. Lower four bits are for indicating change
    // in sub_quads. Upper four bits are for indicating existence of sub_quads.
    uint8_t metadata;

    // Pointers to subquads. If level = 1, points to a leaf. Otherwise, points
    // to other quad instances.
    void **sub_quads;

    // The quad containing the quad, or NULL if quad is top layer.
    struct Quad *parent;
} Quad;

// Initializes a new quad instance.
Quad *quad_init(uint32_t, uint32_t, uint8_t, Quad *);

// Sets the checkbit in the metadata of the sub quad at the given position to
// the given value.
void quad_set_check(Quad *, int, int);

// Gets the checkbit in the metadata of the parent quad representing the given
// quad.
int quad_get_check(Quad *);

// Sets sub quad at given position. Adjusts metadata accordingly.
void quad_set_sub_quad(Quad *, void *, int);

// Returns the position of the quad in the parent quad.
int quad_global_to_local_pos(Quad *);

// Returns the given position in the parent quad.
int global_to_local_pos(uint32_t, uint32_t, uint8_t);

// Frees all memory associated with the quad.
void quad_deinit(Quad *quad);