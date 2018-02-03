#define INITIAL_QUAD_POSITION 3689348814741910323 // 0011001100110011001100110011001100110011001100110011001100110011
#define INITIAL_QUAD_METADATA 255

// Represents a node in a quadtree.
typedef struct Quad {
    // Value indicating the level of the quad. Cell level is 0.
    uint8_t level;
    
    uint32_t x;
    uint32_t y;

    // Represents metadata of subquads. b
    uint8_t metadata;

    // Pointers to subquads. If level = 1, points to a block of cells. Otherwise, points to other quad instances.
    
    // TEMP: 2d matrix of void pointers
    void *** sub_quads;
} Quad;

Quad * quad_init();
void quad_step(Quad*, int);
Leaf * quad_get_leaf(Quad*, uint32_t, uint32_t);
void quad_step_leaf(Quad*, uint32_t, uint32_t, int);