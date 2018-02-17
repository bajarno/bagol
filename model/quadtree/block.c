#include "block.h"

Block block_step(Block data)
{
    Block new_data = data;

    // Loop only over center bits
    for (int x = 1; x < 7; x++)
    {
        for (int y = 1; y < 7; y++)
        {
            // Position of the current bit in the block.
            uint8_t pos = x + y * 8;

            // Use built in count to get amount of bits at 1 in masked block.
            int living = __builtin_popcountll(data & neighbours_mask[pos]);

            // If living is equal to 2, nothing has to change.
            if (living != 2)
            {
                // If exactly three neighbours are alive, set bit to 1 (alive).
                if (living == 3)
                {
                    // Set bit value to 1 (alive).
                    new_data |= bit_mask[pos];
                }
                else
                {
                    // Set bit value to 0 (death).
                    new_data &= bit_unmask[pos];
                }
            }
        }
    }

    return new_data;
}

// Print a block to console in bit form.
void block_print_bits(Block data)
{
    Block mask = 1;
    for (int i = 0; i < 64; i++)
    {
        printf("%d", (data & mask) > 0);

        if ((i + 1) % 8 == 0)
        {
            printf("\n");
        }

        mask *= 2;
    }
}