#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE     64
#define K_PAR           1000
#define INPUT_FILE      "../Inputs/day08.txt"

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))

typedef unsigned long long  ull;
typedef struct pont         point_t;
typedef struct dist         dist_t;

struct pont
{
    size_t x, y, z;
    point_t *next;
    point_t *parent;
    size_t size;
};

struct dist
{
    point_t *p1;
    point_t *p2;
    dist_t  *next;
    ull distance;
};

/* Merge Sorted Functions */
dist_t *merge_sorted(dist_t *a, dist_t *b)
{
    dist_t dummy;
    dist_t *p = &dummy;
    while (a && b)
    {
        if (a->distance <= b->distance)
        { // <= mantém estabilidade
            p->next = a;
            a = a->next;
        }
        else
        {
            p->next = b;
            b = b->next;
        }
        p = p->next;
    }
    p->next = a ? a : b;
    return dummy.next;
}

dist_t *split_in_half(dist_t *head)
{
    if (!head)
        return NULL;
    dist_t *slow = head, *fast = head->next;
    while (fast && fast->next)
    {
        slow = slow->next;
        fast = fast->next->next;
    }
    dist_t *mid = slow->next;
    slow->next = NULL;
    return mid;
}

dist_t *mergesort_rec(dist_t *head)
{
    if (!head || !head->next)
        return head;
    dist_t *mid   = split_in_half(head);
    dist_t *left  = mergesort_rec(head);
    dist_t *right = mergesort_rec(mid);
    return merge_sorted(left, right);
}

/* --- DSU (pointer-based) --- */
inline point_t *dsu_find(point_t *p)
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

/* Union by size, assumes inputs may be any dist_ts (we call find inside) */
inline void dsu_union(point_t *a, point_t *b)
{
    point_t *ra = dsu_find(a);
    point_t *rb = dsu_find(b);
    if (ra == rb)
        return;
    if (ra->size < rb->size)
    {
        ra->parent = rb;
        rb->size  += ra->size;
    }
    else
    {
        rb->parent = ra;
        ra->size  += rb->size;
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
    size_t dist_len   = 0;
    size_t points_len = 0;

    /* read points one by one, create edges to previous points */
    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        point_t *new_point = malloc(sizeof *new_point);
        if (new_point == NULL)
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
            dist_t *new_dist = malloc(sizeof *new_point);
            if (new_dist == NULL)
            {
                perror("malloc");
                exit(1);
            }
            dist_len++;
            new_dist->p1 = new_point;
            new_dist->p2 = other;
            new_dist->next = distances;
            new_dist->distance = dx * dx + dy * dy + dz * dz;
            distances = new_dist;
        }
    }
    fclose(file_in);

    if (points_len == 0)
    {
        fprintf(stderr, "No points read.\n");
        return 1;
    }

    distances = mergesort_rec(distances);

    size_t k_par_used  = dist_len > (size_t)K_PAR ? (size_t)K_PAR : dist_len;
    char printed_part1 = 0;
    ull last_x_product = 0;

    for (dist_t *d = distances; d != NULL; d = d->next)
    {
        point_t *r1 = dsu_find(d->p1);
        point_t *r2 = dsu_find(d->p2);

        if (r1 != r2)
        {
            dsu_union(r1, r2);
            points_len--;
        }

        /* after processing the edge i (0-based), if we've just processed K_PAR edges, print part1 */
        if (!printed_part1 && !k_par_used--)
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
                            max[j] = max[j - 1];
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
        if (points_len == 1)
        {
            /* edges[i] is the edge whose union made points_len==1 (if union happened) */
            last_x_product = (ull)d->p1->x * (ull)d->p2->x;
            printf("[Second answer] Product of last connection %llu\n", last_x_product);
            break;
        }
    }

    /* Coding best practices - Optional */
    while (distances != NULL)
    {
        dist_t *aux = distances;
        distances = distances->next;
        free(aux);
    }
    while (points != NULL)
    {
        point_t *aux = points;
        points = points->next;
        free(aux);
    }
    return 0;
}
