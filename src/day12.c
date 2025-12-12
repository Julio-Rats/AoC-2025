#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_SIZE     64
#define INPUT_FILE      "../Inputs/day12.txt"

int main(int argc, char *argv[])
{
    FILE *file_in = (argc == 1) ? fopen(INPUT_FILE, "r") : fopen(argv[1], "r");
    if (file_in == NULL)
    {
        printf("Error opening file '%s'.\n", argc == 1 ? INPUT_FILE : argv[1]);
        return 1;
    }

    char line[BUFFER_SIZE];
    uint32_t possible_presents = 0;

    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        if (strlen(line) < 5)
            continue;

        char *str, *context;

        str = strtok_r(line, ":", &context);
        uint32_t D1, D2, area = 0;
        sscanf(str, "%zux%zu", &D1, &D2);
        for (int i = 0; i < 5; i++)
            area += strtoul(context, &context, 10);

        if (area * 9 <= D1 * D2)
            possible_presents++;
    }

    printf("[Final answer] Total of presents %zu\n", possible_presents);

    fclose(file_in);
    return 0;
}
