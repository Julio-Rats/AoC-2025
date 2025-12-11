#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE     256
#define ALLOC_SIZE      8
#define INPUT_FILE      "../Inputs/day04.txt"

size_t get_roll_acess(char **roll_matrix, size_t m, size_t n)
{
    if (!roll_matrix || m == 0 || n == 0)
        return 0;

    char *aux = malloc(m * n);
    if (!aux)
        return 0;

    for (size_t i = 0; i < m; ++i)
        memcpy(aux + i * n, roll_matrix[i], n);

    size_t total_removed = 0;

    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < n; ++j)
        {
            if (roll_matrix[i][j] != '@')
                continue;

            size_t neighbors = 0;
            for (int di = -1; di <= 1; ++di)
                for (int dj = -1; dj <= 1; ++dj)
                {
                    if (di == 0 && dj == 0)
                        continue;

                    int ni = (int)i + di;
                    int nj = (int)j + dj;
                    if (ni >= 0 && ni < (int)m && nj >= 0 && nj < (int)n)
                        if (roll_matrix[ni][nj] == '@')
                            neighbors++;
                }
            if (neighbors < 4)
            {
                aux[i * n + j] = '.';
                total_removed++;
            }
        }

    for (size_t i = 0; i < m; ++i)
        memcpy(roll_matrix[i], aux + i * n, n);

    free(aux);
    return total_removed;
}

int main(int argc, char *argv[])
{
    FILE *file_in = (argc == 1) ? fopen(INPUT_FILE, "r") : fopen(argv[1], "r");

    if (file_in == NULL)
    {
        printf("Error opening file '%s'.\n", argc == 1 ? INPUT_FILE : argv[1]);
        return 1;
    }

    char line[BUFFER_SIZE];
    size_t matrix_cap  = 0;
    size_t matrix_len  = 0;
    size_t line_len    = 0;
    char **roll_matrix = NULL;

    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        line_len = strlen(line);

        if (line_len != 0 && line[line_len - 1] == '\n')
            line[--line_len] = '\0';

        line_len++;

        // Create matrix
        if (matrix_len >= matrix_cap)
        {
            // Realloc for more spaces (exponential increase)
            matrix_cap = matrix_cap ? matrix_cap * 2 : ALLOC_SIZE;
            roll_matrix = (char **)realloc(roll_matrix, sizeof *roll_matrix * matrix_cap);
            if (roll_matrix == NULL)
            {
                perror("[ERROR] Realloc");
                exit(1);
            }
        }
        // Alloc space for string
        char *string_matrix = (char *)malloc(sizeof *roll_matrix * line_len);
        roll_matrix[matrix_len++] = string_matrix;

        if (string_matrix == NULL)
        {
            perror("[ERROR] Malloc");
            exit(1);
        }
        // Copy string to matrix of strings
        memcpy(string_matrix, line, sizeof *string_matrix * line_len);
    }
    fclose(file_in);

    // Get results
    size_t first_rolls = get_roll_acess(roll_matrix, matrix_len, line_len);
    size_t next_rolls, total_rolls = first_rolls;

    // keep take out roll if possible
    while ((next_rolls = get_roll_acess(roll_matrix, matrix_len, line_len)) != 0)
        total_rolls += next_rolls;

    printf("[First  Answer] First rolls moved    %u\n", first_rolls);
    printf("[Second Answer] Total of rolls moved %u\n", total_rolls);

    for (size_t i = 0; i < matrix_len; i++)
        free(roll_matrix[i]);
    free(roll_matrix);

    return 0;
}
