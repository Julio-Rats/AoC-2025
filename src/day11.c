#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_SIZE     256
#define MAX_NODES       1024
#define MAX_STR_KEY     8
#define BITMASK_SIZE    4
#define INPUT_FILE      "../Inputs/day11.txt"

#define BITMASK(node, req, bit)    (((node) == (req)) << ((bit) - 1))

typedef struct
{
    char key[MAX_NODES][MAX_STR_KEY];
    uint16_t keys_len;
} map_t;

uint64_t count_paths(uint8_t graph[MAX_NODES][MAX_NODES], uint16_t graph_len, uint16_t start_idx,
                     uint16_t end_idx, uint16_t req1_idx, uint16_t req2_idx)
{
    int16_t degree_in[MAX_NODES] = {0};
    for (uint16_t i = 0; i < graph_len; i++)
        for (uint16_t j = 0; j < graph_len; j++)
            if (graph[i][j])
                degree_in[j]++;

    uint16_t queue[graph_len];
    uint16_t queue_head = 0, queue_tail = 0, topo_tail = 0;
    for (uint16_t i = 0; i < graph_len; i++)
        if (degree_in[i] == 0)
            queue[queue_tail++] = i;

    while (queue_head < queue_tail)
    {
        topo_tail++;
        uint16_t aux = queue[queue_head++];
        for (uint16_t j = 0; j < graph_len; j++)
            if (graph[aux][j] && --degree_in[j] == 0)
                queue[queue_tail++] = j;
    }

    uint64_t dp[MAX_NODES][BITMASK_SIZE] = {0};
    uint8_t start_mask = BITMASK(start_idx, req1_idx, 1) + BITMASK(start_idx, req2_idx, 2);
    dp[start_idx][start_mask] = 1;

    for (uint16_t i = 0; i < topo_tail; i++)
    {
        uint16_t tp = queue[i];
        for (uint8_t j = 0; j < BITMASK_SIZE; j++)
            if (dp[tp][j])
                for (uint16_t k = 0; k < graph_len; k++)
                    if (graph[tp][k])
                    {
                        uint8_t nm = j | BITMASK(k, req1_idx, 1) | BITMASK(k, req2_idx, 2);
                        dp[k][nm] += dp[tp][j];
                    }
    }
    return dp[end_idx][3];
}

uint16_t make_key(map_t *map, const char *key)
{
    strcpy(map->key[map->keys_len++], key);

    if (map->keys_len >= MAX_NODES)
    {
        fprintf(stderr, "\nMap max node\n\n");
        exit(1);
    }
    return map->keys_len - 1;
}

uint16_t get_value(map_t *map, const char *key)
{
    for (size_t i = 0; i < map->keys_len; i++)
        if (strcmp(map->key[i], key) == 0)
            return i;

    return make_key(map, key);
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
    map_t keys = {0};
    uint8_t graph[MAX_NODES][MAX_NODES] = {0};

    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        char *str, *save_context;

        uint16_t line_len = strlen(line);
        if (line_len != 0 && line[line_len - 1] == '\n')
            line[--line_len] = '\0';
        line_len++;

        str = strtok_r(line, ":", &save_context);
        uint16_t n1 = get_value(&keys, str);
        str = strtok_r(save_context, " ", &save_context);
        while (str != NULL)
        {
            uint16_t n2 = get_value(&keys, str);
            graph[n1][n2] = 1;
            str = strtok_r(NULL, " ", &save_context);
        }
    }
    fclose(file_in);

    uint64_t total_simple_path = count_paths(graph, keys.keys_len, get_value(&keys,"you"), 
         get_value(&keys,"out"), get_value(&keys,"you"), get_value(&keys,"out"));

    uint64_t total_compost_path = count_paths(graph, keys.keys_len, get_value(&keys,"svr"), 
        get_value(&keys,"out"), get_value(&keys,"dac"), get_value(&keys,"fft"));

    printf("[First  answer] Total Simples Path %llu\n", total_simple_path);
    printf("[Secnod answer] Total Compost Path %llu\n", total_compost_path);

    return 0;
}