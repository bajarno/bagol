uint64_t NEIGHBOURS_MASK_POS_9 = 460039;

uint64_t INTERNAL_MASK = 35604928818740736;

uint64_t INTERNAL_EDGE_MASK = 35538699412471296; // Masks internal edges of block.
uint64_t INTERNAL_EDGE_N_MASK = 32256; // Masks internal edges of block.
uint64_t INTERNAL_EDGE_NE_MASK = 16384; // Masks internal edges of block.
uint64_t INTERNAL_EDGE_E_MASK = 18085043209519104; // Masks internal edges of block.
uint64_t INTERNAL_EDGE_SE_MASK = 18014398509481984; // Masks internal edges of block.
uint64_t INTERNAL_EDGE_S_MASK = 35465847065542656; // Masks internal edges of block.
uint64_t INTERNAL_EDGE_SW_MASK = 562949953421312; // Masks internal edges of block.
uint64_t INTERNAL_EDGE_W_MASK = 565157600297472; // Masks internal edges of block.
uint64_t INTERNAL_EDGE_NW_MASK = 512; // Masks internal edges of block.

// Represents a block of 8x8 cells.
typedef uint64_t Block;

Block block_step(Block);
void print_bits(Block);