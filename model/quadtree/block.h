#define NEIGHBOURS_MASK_POS_9 460039

// Represents a block of 8x8 cells.
typedef uint64_t Block;

Block block_step(Block);
void print_bits(Block);