// Position of the first quad upon creation of a new tree. Equal for x and y.
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
    SDL_SpinLock read_lock;
} QuadTree;

QuadTree *tree_init();
Leaf *tree_get_leaf(QuadTree *, uint32_t, uint32_t);
void tree_step(QuadTree *);
void tree_step_quad(QuadTree *, Quad *);
void tree_step_leaf(QuadTree *, Leaf *);
void leaf_delete(Leaf *leaf);
void quad_delete(Quad *quad);