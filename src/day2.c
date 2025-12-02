#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE         1024
#define NUMBER_SIZE         32
#define INPUT_FILE         "../Inputs/day2.txt"


int main(int argc, char *argv[])
{
    FILE* file_in;

    if (argc == 1)
        file_in = fopen(INPUT_FILE, "r");
    else
        file_in = fopen(argv[1], "r");

    if (file_in == NULL)
    {
        printf("Error opening file '%s'.\n", argc == 1 ? INPUT_FILE : argv[1]);
        return 1;
    }

    unsigned long long start, end;
    char line[BUFFER_SIZE], number[NUMBER_SIZE];
    unsigned long long half_equal  = 0;
    unsigned long long pattern_mix = 0;

    while(fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        char* token = strtok(line, ",");
        do
        {
            sscanf(token, "%llu-%llu", &start, &end);
            for (unsigned long long i = start; i <= end; i++)
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
        }while ((token = strtok(NULL, ",")) != NULL);
    }

    printf("[First Answer] Total sum of all half-equal dual numbers: %llu\n", half_equal);
    printf("[Second Answer] Total sum of all dual numbers: %llu\n", pattern_mix);

    fclose(file_in);
    return 0;
}
