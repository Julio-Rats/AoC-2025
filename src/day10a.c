#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BUFFER_SIZE     256
#define MAX_LEDS        16
#define MAX_TRANS       1024
#define INPUT_FILE      "../Inputs/day10.txt"

typedef uint16_t word;

typedef struct
{
    word    index[MAX_LEDS];
    uint8_t buttons_len;
} buttons_t;

inline word make_leds(char *str)
{
    word led = 0;
    uint16_t str_len = strlen(str);
    for (size_t i = str_len; i+1 >= 1; i--)
        led = led << 1 | (str[i] == '#' ? 1 : 0);

    return led;
}

inline buttons_t make_buttons(char *str)
{
    buttons_t btts = {0};
    char *externo = NULL;
    char *interno = NULL;

    str = strtok_r(str, ")", &externo);
    while (str != NULL)
    {
        strtok_r(str, "(", &interno);
        str = strtok_r(interno, ",", &interno);
        while (str != NULL)
        {
            btts.index[btts.buttons_len] |= (word)1 << atoi(str);
            str = strtok_r(NULL, ",", &interno);
        }
        str = strtok_r(NULL, ")", &externo);
        if (str)
            btts.buttons_len++;
    }
    return btts;
}

static inline word *bfs_machine(buttons_t buttons, word final_led)
{
    struct node_t
    {
        word visiteds[MAX_TRANS];
        word queue[MAX_TRANS];
        uint16_t index_push;
        uint16_t index_pop;
    } node = {0};
    
    static word parents[MAX_TRANS] = {0};
    word current_led = 0;
    node.index_push  = 1;
    
    node.visiteds[current_led] = 1;
    while (current_led != final_led)
    {
        for (size_t i = 0; i < buttons.buttons_len; i++)
        {
            word aux = current_led ^ buttons.index[i];
            if (node.visiteds[aux] == 0)
            {
                node.queue[node.index_push] = aux;
                node.index_push = (node.index_push+1)%MAX_TRANS;
                node.visiteds[aux] = 1;
                parents[aux] = current_led;
            }
        }
        node.index_pop = (node.index_pop+1)%MAX_TRANS;
        if (node.index_push == node.index_pop)
        {
            printf("Stack underflow\n");
            exit(5);
        }
        current_led = node.queue[node.index_pop];
    }
    return parents;
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
    uint32_t total_states_moved = 0;

    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        char *str, *save;

        str = strtok_r(line, "]", &save);
        word final_led = make_leds(str + 1);
        str = strtok_r(NULL, "{", &save);
        buttons_t buttons = make_buttons(str);
        str = strtok_r(NULL, "}", &save); // Part 2

        word *parents = bfs_machine(buttons, final_led);

        total_states_moved++;
        word aux = final_led;
        while ((aux = parents[aux]) != 0)
            total_states_moved++;

    }

    printf("[First answer] Total Moves: %zu\n", total_states_moved);

    fclose(file_in);
    return 0;
}