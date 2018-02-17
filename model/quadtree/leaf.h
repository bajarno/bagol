// Represents a leaf in a quadtree.
typedef struct Leaf
{
    // Position.
    uint32_t x;
    uint32_t y;

    // An array of length 2 containing the previous and the current generation
    // data. Initially will contain zero blocks (all cells death).
    Block *data;

    // The quad containing the leaf.
    Quad *parent;

    // Local position of the leaf in its parent.
    uint8_t pos_in_parent;
} Leaf;

// Initializes a new leaf instance. Data will be initialized with value 0.
Leaf *leaf_init(uint32_t, uint32_t, Quad *);

// Lets the leaf step one generation.
void leaf_step(Leaf *, int);

// Applies the given mask to the leaf. First the mask is applied, then the data
// is filled in.
void leaf_mask(Leaf *, int, Block, Block);

// Gets the checkbit in the metadata of the parent quad representing the given
// leaf.
int leaf_get_check(Leaf *);

// Returns the given position in the parent quad.
uint8_t global_to_local_pos(uint32_t, uint32_t, uint8_t);

// Frees all memory associated with the leaf.
void leaf_deinit(Leaf *leaf);