#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE     4096
#define MATRIX_LINES    5
#define NUMBER_SIZE     9
#define INPUT_FILE      "../Inputs/day06.txt"

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
    char matrix_input[MATRIX_LINES][BUFFER_SIZE];
    size_t line_len, matrix_len = 0;

    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        line_len = strlen(line);

        if (line_len != 0 && line[line_len - 1] == '\n')
            line[--line_len] = '\0';
        line_len++;

        memcpy(matrix_input[matrix_len++], line, line_len);
    }

    ull honzt_total = 0;
    ull vert_total  = 0;
    ull acc         = 0;
    ull acc_vert    = 0;
    size_t last     = 0;

    char vert_str[NUMBER_SIZE][matrix_len];
    size_t count_number = 0;

    line_len = strlen(matrix_input[0]);
    for (size_t i = 0; i <= line_len; i++)
    {
        char empty_coll = 1;
        for (size_t k = 0; k + 1 < matrix_len; k++)
            if (matrix_input[k][i] != ' ' && matrix_input[0][i] != '\0')
            {
                empty_coll = 0;
                break;
            }

        if (empty_coll)
        {
            count_number = 0;
            for (size_t k = 0; k + 1 < matrix_len; k++)
            {
                matrix_input[k][i] = '\0';
                size_t number;
                if (sscanf(&matrix_input[k][last], "%zu", &number) != 1)
                {
                    fprintf(stderr, "Bad line: %s", line);
                    continue;
                }
                if (k == 0)
                    acc = number;
                else
                    switch (matrix_input[matrix_len - 1][last])
                    {
                    case '*': acc *= number; break;
                    case '+': acc += number; break;
                    }
                matrix_input[k][i] = '@';
            }
            honzt_total += acc;
            vert_total  += acc_vert;
            last = i + 1;
        }
        else
        {
            for (size_t j = 0; j + 1 < matrix_len; j++)
                vert_str[count_number][j] = matrix_input[j][i];

            vert_str[count_number][matrix_len - 1] = '\0';
            size_t number_vert;
            sscanf(vert_str[count_number], "%zu", &number_vert);

            if (count_number++ == 0)
                acc_vert = number_vert;
            else
                switch (matrix_input[matrix_len - 1][last])
                {
                case '*': acc_vert *= number_vert; break;
                case '+': acc_vert += number_vert; break;
                }
        }
    }

    printf("[First  answer] Total %llu\n", honzt_total);
    printf("[Second answer] Total %llu\n", vert_total);

    fclose(file_in);
    return 0;
}
