// Represents a pattern that can be placed.
typedef struct Pattern
{
    char *name;

    int width;
    int height;

    uint8_t **data;
} Pattern;

// Initializes a new pattern instance. Data will be initialized with all death cells.
Pattern *pattern_init(char *, int, int);

// Adds the given pattern to the tree at the given position.
void tree_place_pattern(QuadTree *, Pattern *, uint64_t, uint64_t);

// Prints the pattern to console.
void pattern_print(Pattern *pattern);

// Frees all memory associated with the pattern.
void pattern_deinit(Pattern *pattern);