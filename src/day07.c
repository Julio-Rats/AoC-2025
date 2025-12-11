#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE     256
#define INPUT_FILE      "../Inputs/day07.txt"

typedef unsigned long long ull;

int main(int argc, char *argv[])
{
    FILE *file_in = (argc == 1) ? fopen(INPUT_FILE, "r") : fopen(argv[1], "r");

    if (file_in == NULL)
    {
        printf("Error opening file '%s'.\n", argc == 1 ? INPUT_FILE : argv[1]);
        return 1;
    }
    
    char line[BUFFER_SIZE];
    if (fgets(line, BUFFER_SIZE, file_in) == NULL)
    {
        printf("File does not conform to the expected standard.\n");
        return 2;
    }

    char *c = strchr(line, 'S');
    if (!c)
    {
        printf("Pattern not found!");
        return 2;
    }

    size_t beams_len = strlen(line);

    ull beams[beams_len];
    memset(beams, 0, sizeof *beams * beams_len);
    beams[c - line] = 1;

    ull total_split = 0;
    // Read file
    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        size_t idx = (size_t)-1;
        // Check if the splitter has a laser.
        while ((c = strchr(line + idx + 1, '^')) != NULL)
        {
            idx = c - line;
            ull beam;
            if (idx < beams_len && (beam = beams[idx]))
            {
                ull left_beam  = beams[idx - 1];
                ull right_beam = beams[idx + 1];
                // Propagates the number of paths.
                beams[idx - 1] = left_beam  ? left_beam  + beam : beam;
                beams[idx + 1] = right_beam ? right_beam + beam : beam;

                beams[idx] = 0;
                total_split++;
            }
        }
    }

    ull total_quantum = 0;
    for (size_t i = 0; i < beams_len; i++)
        total_quantum += beams[i];

    printf("[First  answer] Total splits  %llu\n", total_split);
    printf("[Second answer] Total Quantum %llu\n", total_quantum);

    fclose(file_in);
    return 0;
}
