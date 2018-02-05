SDL_Texture *data_texture_init(SDL_Renderer *, uint64_t, uint64_t);
void data_texture_resize(RenderData *, uint64_t, uint64_t);
void data_texture_update_grid(RenderData *, Grid *);
void data_texture_update_tree(RenderData *, QuadTree *);
void data_texture_update_quad(uint16_t *, RenderData *, QuadTree *, Quad *);
void data_texture_update_leaf(uint16_t *, RenderData *, QuadTree *, Leaf *);