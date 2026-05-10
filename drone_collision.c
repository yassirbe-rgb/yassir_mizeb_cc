#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#define N 10000

struct Drone {
    int id;
    float x;
    float y;
    float z;
};

float distance_sq(struct Drone *a, struct Drone *b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dx*dx + dy*dy + dz*dz;
}

int compare_x(const void *a, const void *b) {
    struct Drone *da = (struct Drone *)a;
    struct Drone *db = (struct Drone *)b;
    if (da->x < db->x) return -1;
    if (da->x > db->x) return  1;
    return 0;
}

float closest_pair_rec(struct Drone *essaim, int n, int *id1, int *id2) {
    if (n <= 8) {
        float min_d = FLT_MAX;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                float d = distance_sq(essaim + i, essaim + j);
                if (d < min_d) {
                    min_d = d;
                    *id1 = (essaim + i)->id;
                    *id2 = (essaim + j)->id;
                }
            }
        }
        return min_d;
    }

    int mid = n / 2;
    float mid_x = (essaim + mid)->x;

    int lid1, lid2, rid1, rid2;
    float d_left  = closest_pair_rec(essaim,       mid,   &lid1, &lid2);
    float d_right = closest_pair_rec(essaim + mid, n-mid, &rid1, &rid2);

    float d_min;
    if (d_left < d_right) {
        d_min = d_left;  *id1 = lid1; *id2 = lid2;
    } else {
        d_min = d_right; *id1 = rid1; *id2 = rid2;
    }

    float strip_d = sqrtf(d_min);

    struct Drone *strip = (struct Drone *)malloc(n * sizeof(struct Drone));
    if (!strip) { fprintf(stderr, "malloc strip failed\n"); exit(1); }

    int strip_size = 0;
    for (int i = 0; i < n; i++) {
        float dx = (essaim + i)->x - mid_x;
        if (dx < 0) dx = -dx;
        if (dx < strip_d) {
            *(strip + strip_size) = *(essaim + i);
            strip_size++;
        }
    }

    for (int i = 0; i < strip_size; i++) {
        for (int j = i + 1; j < strip_size; j++) {
            float dy = (strip + j)->y - (strip + i)->y;
            if (dy >= strip_d) break;
            float d = distance_sq(strip + i, strip + j);
            if (d < d_min) {
                d_min  = d;
                strip_d = sqrtf(d_min);
                *id1 = (strip + i)->id;
                *id2 = (strip + j)->id;
            }
        }
    }

    free(strip);
    return d_min;
}

int main(void) {
    srand(42);

    struct Drone *essaim = (struct Drone *)malloc(N * sizeof(struct Drone));
    if (!essaim) {
        fprintf(stderr, "Allocation memoire echouee\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        (essaim + i)->id = i + 1;
        (essaim + i)->x  = ((float)rand() / RAND_MAX) * 10000.0f;
        (essaim + i)->y  = ((float)rand() / RAND_MAX) * 10000.0f;
        (essaim + i)->z  = ((float)rand() / RAND_MAX) * 10000.0f;
    }

    qsort(essaim, N, sizeof(struct Drone), compare_x);

    int id1 = -1, id2 = -1;

    clock_t start = clock();
    float dist_sq = closest_pair_rec(essaim, N, &id1, &id2);
    clock_t end   = clock();

    float dist     = sqrtf(dist_sq);
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;

    printf("Paire la plus proche : Drone %d et Drone %d\n", id1, id2);
    printf("Distance euclidienne : %.4f m\n",  dist);
    printf("Temps d execution    : %.3f ms\n", elapsed);

    free(essaim);
    return 0;
}
