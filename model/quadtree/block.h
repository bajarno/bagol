// Used to mask neighbours of bit at position 9. Can be used for other bits by
// shifting the right amount.
uint64_t NEIGHBOURS_MASK_POS_9 = 460039;

// Used to mask the internal data of a block. Removes the bits in the external
// edge.
uint64_t INTERNAL_MASK = 35604928818740736;
uint64_t INTERNAL_EDGE_MASK = 35538699412471296; // Internal edges.
uint64_t INTERNAL_N_MASK = 32256;                // Internal north edge.
uint64_t INTERNAL_NE_MASK = 16384;               // Internal north-east corner.
uint64_t INTERNAL_E_MASK = 18085043209519104;    // Internal east edge.
uint64_t INTERNAL_SE_MASK = 18014398509481984;   // Internal south-east corner.
uint64_t INTERNAL_S_MASK = 35465847065542656;    // Internal south edge.
uint64_t INTERNAL_SW_MASK = 562949953421312;     // Internal south-west corner.
uint64_t INTERNAL_W_MASK = 565157600297472;      // Internal west edge.
uint64_t INTERNAL_NW_MASK = 512;                 // Internal north-west corner.

// A block represents a 6x6 matrix of cells including the neighbours, resulting
// in a 8x8 matrix of cells. The lowest order bit represents the top left bit in
// the matrix, rest of the bits are in order from left to right and then from
// top to bottom.
typedef uint64_t Block;

// Calculates the next generation for the given block. Will only change internal
// data, but also return external data.
Block block_step(Block);

// Prints a block to terminal in textual form.
void block_print_bits(Block);