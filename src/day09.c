#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BUFFER_SIZE     256
#define MAX_POINTS      512
#define INPUT_FILE      "../Inputs/day09.txt"

#define MIN(a, b)       ((a) < (b) ? (a) : (b))
#define MAX(a, b)       ((a) > (b) ? (a) : (b))
#define SIGN(a, b)      (((a) > (b)) - ((a) < (b)))

typedef struct
{
    uint32_t x, y;
} point_t;

typedef struct
{
    point_t v1;
    point_t v2;
} ret_t;

uint64_t ret_area(point_t p1, point_t p2)
{
    return (uint64_t)(abs((int32_t)p1.x - (int32_t)p2.x) + 1) *
           (uint64_t)(abs((int32_t)p1.y - (int32_t)p2.y) + 1);
}

uint8_t check_ret_collison(ret_t A, ret_t B)
{
    uint8_t A_is_right_B = MIN(A.v1.x, A.v2.x) >= MAX(B.v1.x, B.v2.x);
    uint8_t A_is_left_B  = MAX(A.v1.x, A.v2.x) <= MIN(B.v1.x, B.v2.x);
    uint8_t A_is_above_B = MAX(A.v1.y, A.v2.y) <= MIN(B.v1.y, B.v2.y);
    uint8_t A_is_below_B = MIN(A.v1.y, A.v2.y) >= MAX(B.v1.y, B.v2.y);

    return !(A_is_right_B || A_is_left_B || A_is_above_B || A_is_below_B);
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
    point_t points[MAX_POINTS];

    uint16_t points_len = 0;
    uint32_t x, y;
    
    /* Read Points */
    while (fgets(line, BUFFER_SIZE, file_in) != NULL)
    {
        if (sscanf(line, "%zu,%zu", &x, &y) != 2)
        {
            fprintf(stderr, "Bad line: %s", line);
            continue;
        }

        points[points_len].x = x;
        points[points_len].y = y;
        points_len++;
    }
    fclose(file_in);

    ret_t invalids[MAX_POINTS];
    uint16_t invalids_count = 0;

    /* Generates invalid rectangles (outside the area defined by the points). */
    int8_t dx1 = SIGN(points[1].x, points[0].x);
    int8_t dy1 = SIGN(points[1].y, points[0].y);
    for (uint16_t i = 1; i < points_len; i++)
    {
        uint16_t next = (i + 1) % points_len;

        int8_t dx2 = SIGN(points[next].x, points[i].x);
        int8_t dy2 = SIGN(points[next].y, points[i].y);

        int8_t cross = dx1 * dy2 - dy1 * dx2;

        if (cross < 0)
        {
            invalids[invalids_count].v1 = points[i - 1];
            invalids[invalids_count].v2 = points[next];
            /* Remove overlapping lines from points */
            invalids[invalids_count].v1.y +=  dy2*(!dy1);
            invalids[invalids_count].v1.x +=  dx2*(!dx1);
            invalids[invalids_count].v2.x += -dx1*(!dy1);
            invalids[invalids_count].v2.y += -dy1*(!dx1);

            invalids_count++;
        }
        dx1 = dx2;
        dy1 = dy2;
    }

    uint64_t best_area = 0;
    uint64_t limited_ret_best_are = 0;
    /* 
        Generate all areas, check the largest one for problem 1, 
         and verify the absence of collisions with invalid rectangles.
    */
    for (uint16_t i = 0; i < points_len; i++)
        for (uint16_t j = i + 1; j < points_len; j++)
        {
            uint64_t area;
            uint8_t no_collision = 1;
            if ((area = ret_area(points[i], points[j])) > best_area)
                best_area = area;

            for (uint16_t k = 0; k < invalids_count; k++)
                if (check_ret_collison((ret_t){points[i], points[j]}, invalids[k]))
                {
                    no_collision = 0;
                    break;
                }
            if (no_collision && area > limited_ret_best_are)
                limited_ret_best_are = area;
        }

    printf("[First  answer] Max area %llu\n", best_area);
    printf("[Second answer] Max area %llu\n", limited_ret_best_are);

    return 0;
}
