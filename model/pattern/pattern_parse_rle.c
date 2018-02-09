Pattern *pattern_parse_rle(char *path)
{
    FILE *file = fopen(path, "r");

    Pattern *pattern;

    char *name;
    int width;
    int height;

    char line[200];
    int x = 0;
    int y = 0;
    int multiplier = 0;
    while (fgets(line, 200, file) != NULL)
    {
        // Handle comment lines
        if (line[0] == '#')
        {
            if (line[1] == 'N')
            {
                name = line + 3;
            }
        }
        else if (line[0] == 'x')
        {
            sscanf(line, "x = %d, y = %d", &width, &height);
            pattern = pattern_init(name, width, height);
        }
        else
        {
            for (int i = 0; line[i] != 0; i++)
            {
                switch (line[i])
                {
                case 'b':
                    // If multiplier is zero, set it to the default one for absence of multiplier.
                    multiplier = multiplier ? multiplier : 1;
                    // Since default cells are death, just skip part of the row.
                    x += multiplier;
                    // Reset multiplier.
                    multiplier = 0;
                    break;
                case 'o':
                    // add live cells
                    // If multiplier is zero, set it to the default one for absence of multiplier.
                    multiplier = multiplier ? multiplier : 1;

                    for (int j = 0; j < multiplier; j++)
                    {
                        pattern->data[x][y] = 1;
                        x++;
                    }
                    // Reset multiplier.
                    multiplier = 0;
                    break;
                case '$':
                    // Move to start of new line.
                    // If multiplier is zero, set it to the default one for absence of multiplier.
                    multiplier = multiplier ? multiplier : 1;
                    x = 0;
                    y += multiplier;
                    // Reset multiplier.
                    multiplier = 0;
                    break;
                case '!':
                    goto EndOfFile;
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    multiplier = 10 * multiplier + (line[i] - '0');
                    break;
                default:
                    continue;
                }
            }
        }
    }

EndOfFile:;
    // End of file, close file and return pattern.
    fclose(file);
    return pattern;
}