typedef enum {
    // Oscillators
    BLINKER,

    // Spaceships
    GLIDER,

    // Other
    RPENTOMINO
} Pattern;

void add_pattern(Pattern, Grid*, int, int);