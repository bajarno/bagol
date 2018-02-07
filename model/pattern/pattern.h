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