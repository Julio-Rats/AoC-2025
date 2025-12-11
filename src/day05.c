#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE         64
#define INPUT_FILE          "../Inputs/day5.txt"

#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define MAX(a, b)           ((a) > (b) ? (a) : (b))
#define IS_OVERLAP(r1, r2)  (MAX((r1)->lower, (r2)->lower) <= MIN((r1)->upper, (r2)->upper))

typedef unsigned long long ull;
typedef struct range range_t;

struct range
{
    ull lower;
    ull upper;
    char disregarded;
    range_t *next;
    range_t *prev;
};


int main(int argc, char *argv[])
{
    FILE *file_in = (argc == 1) ? fopen(INPUT_FILE, "r") : fopen(argv[1], "r");

    if (file_in == NULL)
    {
        printf("Error opening file '%s'.\n", argc == 1 ? INPUT_FILE : argv[1]);
        return 1;
    }

    range_t *ranges = NULL;
    char line[BUFFER_SIZE];

    // Make ranges
    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        if (*line == '\n') break;

        ull lower, upper;
        if (sscanf(line, "%llu-%llu", &lower, &upper) != 2)
        {
            fprintf(stderr, "Bad line: %s", line);
            continue;
        }
        range_t *new_range = (range_t *)malloc(sizeof *new_range);
        if (new_range == NULL)
        {
            perror("Malloc");
            exit(1);
        }
        new_range->lower = lower;
        new_range->upper = upper;
        new_range->disregarded = 0;
        new_range->next = ranges;
        new_range->prev = NULL;
        if (ranges != NULL)
            ranges->prev = new_range;
        ranges = new_range;
    }

    // Merge ranges
    range_t *r = ranges;
    while (r != NULL)
    {
        range_t *next = r->next;
        while (r->disregarded == 0 && next != NULL)
        {
            // Merge available
            if (next->disregarded == 0 && IS_OVERLAP(r, next))
            {
                next->lower = MIN(r->lower, next->lower);
                next->upper = MAX(r->upper, next->upper);
                r->disregarded = 1;
                continue;
            }
            next = next->next;
        }
        range_t *aux = r;
        r = r->next;
        if (aux->disregarded != 0)
        {
            if (r != NULL)
                r->prev = aux->prev;
            if (aux->prev != NULL)
                aux->prev->next = r;
            if (ranges == aux)
                ranges = r;
            free(aux);
        }
    }

    ull available_ids = 0;
    // Get and compare IDs for part 1
    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        ull id;
        if (sscanf(line, "%llu", &id) != 1)
        {
            fprintf(stderr, "Bad line: %s", line);
            continue;
        }
        r = ranges;
        while (r != NULL)
        {
            if (r->lower <= id && id <= r->upper)
            {
                available_ids++;
                break;
            }
            r = r->next;
        }
    }

    ull total_available_ids = 0;
    // Free ranges and count total ids
    r = ranges;
    while (r != NULL)
    {
        total_available_ids += r->upper - r->lower + 1;

        range_t *aux = r;
        r = r->next;
        free(aux);
    }

    printf("[First  answer] Total Ids Available %llu\n", available_ids);
    printf("[Second answer] Total Ids Available %llu\n", total_available_ids);

    fclose(file_in);
    return 0;
}
