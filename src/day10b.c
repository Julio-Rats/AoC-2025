#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_SIZE         256
#define MAX_VARS            15
#define MAX_BUTTONS         20
#define MAX_PATTERN_MASKS   65536
#define HASH_SIZE           131072
#define INF                 1e6
#define INPUT_FILE          "../Inputs/day10.txt"

typedef struct
{
    int32_t d[MAX_VARS];
} Vector;

typedef struct
{
    Vector  key;
    int32_t value;
    enum
    {
        EMPTY,
        USED
    } status;
} HashEntry;

size_t N, M;
HashEntry memo[HASH_SIZE];
Vector patterns[MAX_PATTERN_MASKS];
size_t costs[MAX_PATTERN_MASKS];
size_t p_count = 0;

char is_zero(Vector v)
{
    for (size_t i = 0; i < N; i++)
    {
        if (v.d[i] != 0)
            return 0;
    }
    return 1;
}

int32_t *get_memo(Vector key)
{
    unsigned h = 0;
    for (size_t i = 0; i < N; i++)
    {
        h = h * 31 + key.d[i];
    }
    h &= (HASH_SIZE - 1);

    while (1)
    {
        if (memo[h].status == EMPTY)
        {
            memo[h].key = key;
            memo[h].status = USED;
            memo[h].value = -1;
            return &memo[h].value;
        }
        if (memcmp(memo[h].key.d, key.d, N * sizeof(int)) == 0)
        {
            return &memo[h].value;
        }
        h = (h + 1) & (HASH_SIZE - 1);
    }
}

uint32_t solve(Vector goal)
{
    if (is_zero(goal))
        return 0;

    int32_t *cached = get_memo(goal);
    if (*cached != -1)
    {
        return *cached;
    }

    uint32_t res = INF;
    for (size_t i = 0; i < p_count; i++)
    {
        Vector next;
        char valid = 1;

        for (size_t j = 0; j < N; j++)
        {
            int32_t diff = goal.d[j] - patterns[i].d[j];

            if (diff < 0 || (diff % 2) != 0)
            {
                valid = 0;
                break;
            }
            next.d[j] = diff / 2;
        }

        if (valid)
        {
            uint32_t recursive_res = solve(next);
            if (recursive_res != INF)
            {
                uint32_t current_cost = costs[i] + 2 * recursive_res;
                if (current_cost < res)
                    res = current_cost;
            }
        }
    }

    return *cached = res;
}

void precompute(uint8_t coeffs[][MAX_VARS])
{
    p_count = 0;
    memset(memo, 0, sizeof(memo));

    for (size_t mask = 0; mask < (size_t)(1 << M); mask++)
    {
        Vector current_sum  = {0};
        size_t current_cost = 0;

        for (size_t i = 0; i < M; i++)
            if ((mask >> i) & 1)
            {
                current_cost++;
                for (size_t j = 0; j < N; j++)
                    current_sum.d[j] += coeffs[i][j];
            }

        long found_idx = -1;
        for (size_t k = 0; k < p_count; k++)
            if (memcmp(patterns[k].d, current_sum.d, N * sizeof(int)) == 0)
            {
                found_idx = k;
                break;
            }

        if (found_idx != -1)
        {
            if (current_cost < costs[found_idx])
                costs[found_idx] = current_cost;
        }
        else
        {
            patterns[p_count] = current_sum;
            costs[p_count++] = current_cost;
        }
    }
}

void parse_input_line(char *line, uint8_t coeffs[][MAX_VARS], int32_t *goal_arr, size_t *n_out, size_t *m_out)
{
    *n_out = 0;
    *m_out = 0;

    char *p_brace = strchr(line, '{');
    if (!p_brace)
        return;

    char *curr = p_brace + 1;
    while (*curr && *curr != '}')
    {
        goal_arr[(*n_out)++] = (int32_t)strtol(curr, &curr, 10);
        if (*curr == ',')
            curr++;
    }

    curr = line;
    while ((curr = strchr(curr, '(')) && curr < p_brace)
    {
        curr++;

        while (*curr && *curr != ')')
        {
            size_t idx = (int)strtol(curr, &curr, 10);
            if (idx < *n_out)
                coeffs[*m_out][idx] = 1;
            if (*curr == ',')
                curr++;
        }
        (*m_out)++;
    }
}

int main(int argc, char *argv[])
{
    const char *filename = (argc == 1) ? INPUT_FILE : argv[1];
    FILE *file_in = fopen(filename, "r");

    if (file_in == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo '%s'.\n", filename);
        return 1;
    }

    char line[BUFFER_SIZE];
    uint64_t total_ans = 0;

    while (fgets(line, sizeof(line), file_in))
    {
        uint8_t coeffs[MAX_BUTTONS][MAX_VARS] = {0};
        int32_t goal_arr[MAX_VARS];

        parse_input_line(line, coeffs, goal_arr, &N, &M);

        if (N == 0 || M == 0)
            continue;

        precompute(coeffs);

        Vector goal;
        memcpy(goal.d, goal_arr, N * sizeof(int));

        uint32_t result = solve(goal);

        if (result < INF)
            total_ans += result;
    }

    printf("[Second answer] Total button pressed %llu\n", total_ans);

    fclose(file_in);
    return 0;
}