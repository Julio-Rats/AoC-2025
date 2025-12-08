#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_POSITION      50
#define BUFFER_SIZE         16
#define INPUT_FILE         "../Inputs/day1.txt"

int main(int argc, char *argv[])
{
    FILE *file_in = (argc == 1) ? fopen(INPUT_FILE, "r") : fopen(argv[1], "r");

    if (file_in == NULL)
    {
        printf("Error opening file '%s'.\n", argc == 1 ? INPUT_FILE : argv[1]);
        return 1;
    }

    int old_position, position = START_POSITION;
    size_t pass_zero = 0;
    size_t set_zero  = 0;
    size_t offset;
    char line[BUFFER_SIZE];

    while(fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        old_position = position;
        offset = (unsigned)atoi(&line[1]);
        pass_zero += offset / 100;

        if ((offset = offset % 100) == 0)
        {
            if (position == 0)
                set_zero++;
            continue;
        }

        position += (line[0] == 'L') ? -offset : offset;

        if (old_position != 0 && (position >= 100 || position <= 0))
            pass_zero++;
        
        position = (position + 100) % 100;
        if (position == 0)
            set_zero++;
    }

    printf("[First  Answer] Number of times stop in zero                        %zu\n", set_zero);
    printf("[Second Answer] Number of times pass through zero plus stop in zero %zu\n", pass_zero);

    fclose(file_in);
    return 0;
}
