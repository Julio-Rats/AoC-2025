#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE     256
#define ALLOC_SIZE      8
#define INPUT_FILE      "../Inputs/day4.txt"

size_t get_roll_acess(char **roll_matrix, size_t m, size_t n)
{
    // make Backup
    char aux_remove[m][n];
    for (size_t i = 0; i < m; i++)
        memcpy(aux_remove[i], roll_matrix[i], sizeof **roll_matrix * n);

    size_t total_roll = 0;
    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < n; j++)
            if (roll_matrix[i][j] == '@')
            {
                size_t count_roll = 0;
                // check same line
                if (j != 0)
                    count_roll += roll_matrix[i][j - 1] == '@' ? 1 : 0;
                if (j + 1 != m)
                    count_roll += roll_matrix[i][j + 1] == '@' ? 1 : 0;
                // check the next and back lines
                size_t start = j     != 0 ? j - 1 : 0;
                size_t end   = j + 1 != m ? j + 1 : m - 1;
                // back line exist
                if (i != 0)
                    for (size_t k = start; k <= end; k++)
                        count_roll += roll_matrix[i - 1][k] == '@' ? 1 : 0;
                if (count_roll > 3)
                    continue;
                // next line exist
                if (i + 1 != m)
                    for (size_t k = start; k <= end; k++)
                        count_roll += roll_matrix[i + 1][k] == '@' ? 1 : 0;

                if (count_roll < 4)
                {
                    aux_remove[i][j] = '.';
                    total_roll++;
                }
            }
    }
    // transfer the removed matrix to the original matrix
    for (size_t i = 0; i < m; i++)
        memcpy(roll_matrix[i], aux_remove[i], sizeof(char) * n);

    return total_roll;
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

    char line[BUFFER_SIZE];
    size_t matrix_cap = 0;
    size_t matrix_len = 0;
    size_t line_len = 0;
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
            ;
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

    // Get results
    size_t first_rolls = get_roll_acess(roll_matrix, matrix_len, line_len);
    size_t next_rolls, total_rolls = first_rolls;

    // keep take out roll if possible
    while ((next_rolls = get_roll_acess(roll_matrix, matrix_len, line_len)) != 0)
        total_rolls += next_rolls;

    printf("[First Answer]  First rolls moved:    %u\n", first_rolls);
    printf("[Second Answer] Total of rolls moved: %u\n", total_rolls);

    for (size_t i = 0; i < matrix_len; i++)
        free(roll_matrix[i]);
    free(roll_matrix);

    fclose(file_in);
    return 0;
}
