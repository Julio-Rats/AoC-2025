#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE     64
#define HEAP_ALLOC      1024
#define K_PAR           1000
#define INPUT_FILE      "../Inputs/day8.txt"

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))

typedef unsigned long long  ull;
typedef struct pont         point_t;

struct pont
{
    size_t x, y, z;
    point_t *next;
    point_t *parent;
    size_t size;
};

typedef struct
{
    point_t *p1;
    point_t *p2;
    ull distance;
} dist_t;

void dist_push(dist_t **distances, size_t *n, size_t *cap, dist_t dist)
{
    if (*n >= *cap)
    {
        *cap = *cap ? *cap * 2 : HEAP_ALLOC;
        *distances = realloc(*distances, sizeof **distances * *cap);
        if (*distances == NULL)
        {
            perror("realloc");
            exit(1);
        }
    }
    (*distances)[(*n)++] = dist;
}

/* --- DSU (pointer-based) --- */
point_t *dsu_find(point_t *p)
{
    point_t *root = p;
    while (root->parent != root)
        root = root->parent;
    /* path compression */
    while (p->parent != root)
    {
        point_t *next = p->parent;
        p->parent = root;
        p = next;
    }
    return root;
}

/* union by size, assumes inputs may be any nodes (we call find inside) */
void dsu_union(point_t *a, point_t *b)
{
    point_t *ra = dsu_find(a);
    point_t *rb = dsu_find(b);
    if (ra == rb)
        return;
    if (ra->size < rb->size)
    {
        ra->parent = rb;
        rb->size += ra->size;
    }
    else
    {
        rb->parent = ra;
        ra->size += rb->size;
    }
}

/* comparator */
int cmp_dist(const void *A, const void *B)
{
    const dist_t *a = (dist_t *)A;
    const dist_t *b = (dist_t *)B;
    return (a->distance > b->distance) ? 1 : ((a->distance < b->distance) ? -1 : 0);
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
    dist_t *distances = NULL;
    point_t *points   = NULL;
    size_t dist_cap   = 0;
    size_t dist_len   = 0;
    size_t points_len = 0;

    /* read points one by one, create edges to previous points */
    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        point_t *new_point = malloc(sizeof *new_point);
        if (!new_point)
        {
            perror("malloc");
            exit(1);
        }
        if (sscanf(line, "%zu,%zu,%zu", &new_point->x, &new_point->y, &new_point->z) != 3)
        {
            fprintf(stderr, "Bad line: %s", line);
            free(new_point);
            continue;
        }
        points_len++;

        new_point->next = points;
        /* DSU init here */
        new_point->parent = new_point;
        new_point->size = 1;
        points = new_point;

        /* Generate edges */
        for (point_t *other = new_point->next; other != NULL; other = other->next)
        {
            ull dx = (ull)(MAX(new_point->x, other->x) - MIN(new_point->x, other->x));
            ull dy = (ull)(MAX(new_point->y, other->y) - MIN(new_point->y, other->y));
            ull dz = (ull)(MAX(new_point->z, other->z) - MIN(new_point->z, other->z));
            dist_t dist;
            dist.p1 = new_point;
            dist.p2 = other;
            dist.distance = dx * dx + dy * dy + dz * dz;
            dist_push(&distances, &dist_len, &dist_cap, dist);
        }
    }
    fclose(file_in);

    if (points_len == 0)
    {
        fprintf(stderr, "No points read.\n");
        if (distances != NULL)
            free(distances);
        return 1;
    }

    qsort(distances, dist_len, sizeof *distances, cmp_dist);

    size_t k_par_used  = dist_len > (size_t)K_PAR ? (size_t)K_PAR : dist_len;
    size_t components  = points_len;
    char printed_part1 = 0;
    ull last_x_product = 0;

    for (size_t i = 0; i < dist_len; ++i)
    {
        point_t *r1 = dsu_find(distances[i].p1);
        point_t *r2 = dsu_find(distances[i].p2);

        if (r1 != r2)
        {
            dsu_union(r1, r2);
            components--;
        }

        /* after processing the edge i (0-based), if we've just processed K_PAR edges, print part1 */
        if (!printed_part1 && i + 1 >= k_par_used)
        {
            /* scan roots and pick top-3 sizes (O(n), done only once) */
            size_t max[3] = {0};
            for (point_t *p = points; p != NULL; p = p->next)
            {
                if (p->parent != p)
                    continue;
                size_t s = p->size;
                for (size_t i = 0; i < 3; i++)
                    if (s > max[i]) 
                    {
                        for (size_t j = 2; j > i; j--)
                            max[j] = max[j-1];
                        max[i] = s;
                        break;
                    }
            }

            last_x_product = (ull)max[0] * (ull)max[1] * (ull)max[2];
            printf("[First  answer] Top-3 product              %llu\n", last_x_product);
            printed_part1 = 1;
            /* if dist_len < K_PAR we still printed; and we keep going */
        }

        /* check if this union just made everything connected */
        if (components == 1)
        {
            /* edges[i] is the edge whose union made components==1 (if union happened) */
            last_x_product = (ull)distances[i].p1->x * (ull)distances[i].p2->x;
            printf("[Second answer] Product of last connection %llu\n", last_x_product);
            break;
        }
    }

    free(distances);
    while (points != NULL)
    {
        point_t *aux = points;
        points = points->next;
        free(aux);
    }
    return 0;
}
