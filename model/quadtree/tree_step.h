typedef struct QuadStepData
{
    QuadTree *tree;
    Quad *quad;
} QuadStepData;

// Calculates the next generation for all the leafs in the given tree.
void tree_step(QuadTree *);

// Calculates the next generation for all the leafs in the given quad.
int tree_step_quad(void *);

// Calculates the next generation the leaf.
void tree_step_leaf(QuadTree *, Leaf *);