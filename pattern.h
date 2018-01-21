typedef enum Pattern {
    // Oscillators
    BLINKER,

    // Spaceships
    GLIDER,

    // Other
    RPENTOMINO
} Pattern;

void add_pattern(Pattern, Grid*, int, int);
void set_state(Grid*, int, int, uint8_t);