// Represents a leaf in a quadtree.
typedef struct Leaf
{
    uint32_t x;
    uint32_t y;

    // An array of length 2 containing the previous and the current generation data
    Block *data;

    Quad *parent;
} Leaf;

Leaf *leaf_init(uint32_t, uint32_t, Quad *);
void leaf_step(Leaf *, int);
void leaf_mask(Leaf *, int, Block, Block);
int leaf_global_to_local_pos(Leaf *);