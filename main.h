typedef enum DataStructure {
    GRID,
    QUADTREE
} DataStructure;

// Struct representing draw data.
typedef struct AppData
{
    DataStructure data_structure;

    Grid *grid;
    QuadTree *tree;

    RenderData *render_data;

    float fps;
    float ups;

    short quit;
} AppData;

int main(int, char **);
int draw_loop(void *);
int event_loop(void *);
int update_loop(void *);