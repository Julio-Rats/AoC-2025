#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE     256
#define INPUT_FILE      "../Inputs/day3.txt"

typedef unsigned long long ull;

ull get_max_joltage(char *line, size_t line_len, size_t num_batteries)
{
    if (line_len <= num_batteries)
    {
        printf("\n[Error]: Number of digits must be greater than the number of batteries.\n\n");
        exit(1);
    }

    ull total_joltage = 0;

    ssize_t limit = -1;
    for (ssize_t i = num_batteries - 1; i >= 0; i--)
    {
        size_t index_max = 0;
        char digit_max   = 0;

        for (size_t j = limit + 1; j < line_len - i; j++)
            if (line[j] > digit_max)
            {
                index_max = j;
                digit_max = line[j];
            }

        limit = index_max;
        total_joltage = total_joltage * 10 + digit_max - '0';
    }

    return total_joltage;
}

int main(int argc, char *argv[])
{
    FILE *file_in;

    if (argc == 1)
        file_in = fopen(INPUT_FILE, "r");
    else
        file_in = fopen(argv[1], "r");

    if (file_in == NULL)
    {
        printf("Error opening file '%s'.\n", argc == 1 ? INPUT_FILE : argv[1]);
        return 1;
    }

    ull twice_batts  = 0;
    ull twelve_batts = 0;
    char line[BUFFER_SIZE];

    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        size_t line_len = strlen(line);
        if (line_len != 0 && line[line_len - 1] == '\n')
            line[line_len - 1] = '\0';

        line_len = strlen(line);
        twice_batts  += get_max_joltage(line, line_len, 2);
        twelve_batts += get_max_joltage(line, line_len, 12);
    }

    printf("[First Answer]  Total of choques for  2 batts: %llu\n", twice_batts);
    printf("[Second Answer] Total of choques for 12 batts: %llu\n", twelve_batts);

    fclose(file_in);
    return 0;
}
