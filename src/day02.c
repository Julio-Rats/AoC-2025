#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE         1024
#define NUMBER_SIZE         32
#define INPUT_FILE         "../Inputs/day2.txt"

typedef unsigned long long ull;

int main(int argc, char *argv[])
{
    FILE *file_in = (argc == 1) ? fopen(INPUT_FILE, "r") : fopen(argv[1], "r");

    if (file_in == NULL)
    {
        printf("Error opening file '%s'.\n", argc == 1 ? INPUT_FILE : argv[1]);
        return 1;
    }

    char line[BUFFER_SIZE], number[NUMBER_SIZE];
    ull half_equal  = 0;
    ull pattern_mix = 0;
    ull start, end;
    
    while(fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        char* token = strtok(line, ",");
        do
        {
            if (sscanf(token, "%llu-%llu", &start, &end) != 2)
            {
                fprintf(stderr, "Bad line: %s", line);
                continue;
             }
            for (ull i = start; i <= end; i++)
            {
                size_t number_len = snprintf(number, sizeof(number), "%llu", i);
                size_t pattern_len = 1;
                if (number_len % 2 == 0)
                    if (strncmp(number, number + number_len / 2, number_len / 2) == 0)
                        half_equal += i;
                        
                while (pattern_len <= number_len / 2)
                {
                    if (number_len % pattern_len == 0)
                    {
                        char is_dual = 1;
                        for (size_t pos = pattern_len; pos < number_len; pos += pattern_len)
                            if (strncmp(number, number + pos, pattern_len) != 0)
                            {
                                is_dual = 0;
                                break;
                            }
                        if (is_dual)
                        {
                            pattern_mix += i;
                            break;
                        }
                    }
                    pattern_len++;
                }
            }
        } while ((token = strtok(NULL, ",")) != NULL);
    }

    printf("[First  Answer] Total sum of all half-equal dual numbers %llu\n", half_equal);
    printf("[Second Answer] Total sum of all dual numbers            %llu\n", pattern_mix);

    fclose(file_in);
    return 0;
}
