// Position of the first quad upon creation of a new tree. Equal for x and y.
uint32_t INITIAL_QUAD_POSITION = 2863311530;

// Represents a quadtree.
typedef struct QuadTree {
    // Value indicating which position in the list of generation data in the leafs is current.
    uint8_t current_gen;

    // Current parent quad i.e. top of the tree.
    Quad * parent_quad;
} QuadTree;

QuadTree * tree_init();
Leaf * tree_get_leaf(QuadTree*, uint32_t, uint32_t);