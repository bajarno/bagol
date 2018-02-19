// Position of the first quad upon creation of a new tree. Equal for x and y.
// Is equal to binary value 10101010101010101010101010101010. Expanding from
// this initial position will result in the minimal amount of layers needing to
// be added.
uint32_t INITIAL_QUAD_POSITION = 2863311530;

// Represents a quadtree.
typedef struct QuadTree
{
    // Value indicating which position in the list of generation data in the leafs is current.
    uint8_t current_gen;

    // Current parent quad i.e. top of the tree.
    Quad *parent_quad;

    // SpinLock for reading and writing to data, used for blocking access by other threads.
    SDL_SpinLock write_lock;
} QuadTree;

// Initializes a new quadtree instance.
QuadTree *tree_init();

// Retrieves the leaf of the given quadtree at the given position. If the leaf
// does not exist, the tree is expanded and the leaf is added. Initial data for
// a newly created leaf is 0 (All cells death).
Leaf *tree_get_leaf(QuadTree *, uint32_t, uint32_t);

// Sets leaf at the given position in the given tree to the new data. Current
// generation data will be overwritten. Only the internal bits of the leaf at
// the given position are overwritten, external bits of neighbouring leafs are
// adjusted accordingly. Nonexisting leafs are created.
void tree_set_leaf(QuadTree *, uint32_t, uint32_t, Block);

// Deletes the given leaf from its tree. After deleting the leaf, its parent
// quad is also checked for deletion.
void tree_delete_leaf(Leaf *leaf);

// Deletes the given quad from its tree if possible.
void tree_delete_quad(Quad *quad);