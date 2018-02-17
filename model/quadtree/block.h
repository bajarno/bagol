// A block represents a 6x6 matrix of cells including the neighbours, resulting
// in a 8x8 matrix of cells. The lowest order bit represents the top left bit in
// the matrix, rest of the bits are in order from left to right and then from
// top to bottom.
typedef uint64_t Block;

// Lookup table for mask of neighbours of a bit position in a block. Position in
// the block corresponds to position in the array.
const Block neighbours_mask[] = {
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    460039ULL,
    920078ULL,
    1840156ULL,
    3680312ULL,
    7360624ULL,
    14721248ULL,
    0,
    0,
    117769984ULL,
    235539968ULL,
    471079936ULL,
    942159872ULL,
    1884319744ULL,
    3768639488ULL,
    0,
    0,
    30149115904ULL,
    60298231808ULL,
    120596463616ULL,
    241192927232ULL,
    482385854464ULL,
    964771708928ULL,
    0,
    0,
    7718173671424ULL,
    15436347342848ULL,
    30872694685696ULL,
    61745389371392ULL,
    123490778742784ULL,
    246981557485568ULL,
    0,
    0,
    1975852459884544ULL,
    3951704919769088ULL,
    7903409839538176ULL,
    15806819679076352ULL,
    31613639358152704ULL,
    63227278716305408ULL,
    0,
    0,
    505818229730443264ULL,
    1011636459460886528ULL,
    2023272918921773056ULL,
    4046545837843546112ULL,
    8093091675687092224ULL,
    16186183351374184448ULL,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

// Lookup table for mask of a bit position in a block. Position in the block
// corresponds to position in the array.
const Block bit_mask[] = {
    1ULL,
    2ULL,
    4ULL,
    8ULL,
    16ULL,
    32ULL,
    64ULL,
    128ULL,
    256ULL,
    512ULL,
    1024ULL,
    2048ULL,
    4096ULL,
    8192ULL,
    16384ULL,
    32768ULL,
    65536ULL,
    131072ULL,
    262144ULL,
    524288ULL,
    1048576ULL,
    2097152ULL,
    4194304ULL,
    8388608ULL,
    16777216ULL,
    33554432ULL,
    67108864ULL,
    134217728ULL,
    268435456ULL,
    536870912ULL,
    1073741824ULL,
    2147483648ULL,
    4294967296ULL,
    8589934592ULL,
    17179869184ULL,
    34359738368ULL,
    68719476736ULL,
    137438953472ULL,
    274877906944ULL,
    549755813888ULL,
    1099511627776ULL,
    2199023255552ULL,
    4398046511104ULL,
    8796093022208ULL,
    17592186044416ULL,
    35184372088832ULL,
    70368744177664ULL,
    140737488355328ULL,
    281474976710656ULL,
    562949953421312ULL,
    1125899906842624ULL,
    2251799813685248ULL,
    4503599627370496ULL,
    9007199254740992ULL,
    18014398509481984ULL,
    36028797018963968ULL,
    72057594037927936ULL,
    144115188075855872ULL,
    288230376151711744ULL,
    576460752303423488ULL,
    1152921504606846976ULL,
    2305843009213693952ULL,
    4611686018427387904ULL,
    9223372036854775808ULL,
};

// Lookup table for mask of all bits but one in a block. Position in the block
// of the bit that should not be masked corresponds to position in the array.
const Block bit_unmask[] = {
    18446744073709551614ULL,
    18446744073709551613ULL,
    18446744073709551611ULL,
    18446744073709551607ULL,
    18446744073709551599ULL,
    18446744073709551583ULL,
    18446744073709551551ULL,
    18446744073709551487ULL,
    18446744073709551359ULL,
    18446744073709551103ULL,
    18446744073709550591ULL,
    18446744073709549567ULL,
    18446744073709547519ULL,
    18446744073709543423ULL,
    18446744073709535231ULL,
    18446744073709518847ULL,
    18446744073709486079ULL,
    18446744073709420543ULL,
    18446744073709289471ULL,
    18446744073709027327ULL,
    18446744073708503039ULL,
    18446744073707454463ULL,
    18446744073705357311ULL,
    18446744073701163007ULL,
    18446744073692774399ULL,
    18446744073675997183ULL,
    18446744073642442751ULL,
    18446744073575333887ULL,
    18446744073441116159ULL,
    18446744073172680703ULL,
    18446744072635809791ULL,
    18446744071562067967ULL,
    18446744069414584319ULL,
    18446744065119617023ULL,
    18446744056529682431ULL,
    18446744039349813247ULL,
    18446744004990074879ULL,
    18446743936270598143ULL,
    18446743798831644671ULL,
    18446743523953737727ULL,
    18446742974197923839ULL,
    18446741874686296063ULL,
    18446739675663040511ULL,
    18446735277616529407ULL,
    18446726481523507199ULL,
    18446708889337462783ULL,
    18446673704965373951ULL,
    18446603336221196287ULL,
    18446462598732840959ULL,
    18446181123756130303ULL,
    18445618173802708991ULL,
    18444492273895866367ULL,
    18442240474082181119ULL,
    18437736874454810623ULL,
    18428729675200069631ULL,
    18410715276690587647ULL,
    18374686479671623679ULL,
    18302628885633695743ULL,
    18158513697557839871ULL,
    17870283321406128127ULL,
    17293822569102704639ULL,
    16140901064495857663ULL,
    13835058055282163711ULL,
    9223372036854775807ULL,
};

// Used to mask the internal data of a block. Removes the bits in the external
// edge.
const uint64_t INTERNAL_MASK = 35604928818740736;
const uint64_t INTERNAL_EDGE_MASK = 35538699412471296; // Internal edges.
const uint64_t INTERNAL_N_MASK = 32256;                // Internal north edge.
const uint64_t INTERNAL_NE_MASK = 16384;               // Internal north-east corner.
const uint64_t INTERNAL_E_MASK = 18085043209519104;    // Internal east edge.
const uint64_t INTERNAL_SE_MASK = 18014398509481984;   // Internal south-east corner.
const uint64_t INTERNAL_S_MASK = 35465847065542656;    // Internal south edge.
const uint64_t INTERNAL_SW_MASK = 562949953421312;     // Internal south-west corner.
const uint64_t INTERNAL_W_MASK = 565157600297472;      // Internal west edge.
const uint64_t INTERNAL_NW_MASK = 512;                 // Internal north-west corner.

// Used to mask the external data of a block.
const uint64_t EXTERNAL_MASK = 18411139144890810879ULL;

// Calculates the next generation for the given block. Will only change internal
// data, but also return external data.
Block block_step(Block);

// Prints a block to terminal in textual form.
void block_print_bits(Block);