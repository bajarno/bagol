#include "block.h"

Block block_step(Block data)
{
    Block new_data = data;

    // Loop only over center bits
    for (int x = 1; x < 7; x++)
    {
        for (int y = 1; y < 7; y++)
        {
            uint8_t pos = x + y * 8;

            // Create mask to set everything other than neighbouring cells to 0.
            Block mask = NEIGHBOURS_MASK_POS_9 << (pos - 9);
            Block masked_neighbours = data & mask;

            // Use built in count to get amount of bits at 1 in masked block.
            int living = __builtin_popcountll(masked_neighbours);

            // If living is equal to 2, nothing has to change.
            if (living != 2)
            {
                // Calculate values for masking and unmasking current bit.
                Block bit_mask = 1;
                bit_mask <<= pos;
                Block bit_unmask = bit_mask ^ -1;

                // Set bit value to 0 (death).
                new_data &= bit_unmask;

                // If exactly three neighbours are alive, set bit to 1 (alive).
                if (living == 3)
                {
                    new_data |= bit_mask;
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