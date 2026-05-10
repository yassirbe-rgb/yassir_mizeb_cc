#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#define N 10000

// Structure représentant un drone
struct Drone {
    int id;
    float x;
    float y;
    float z;
};

// Calcul de la distance euclidienne au carré
// (plus rapide que sqrt à chaque comparaison)
float distance_sq(struct Drone *a, struct Drone *b) {

    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;

    return dx * dx + dy * dy + dz * dz;
}

// Fonction utilisée par qsort pour trier selon x
int compare_x(const void *a, const void *b) {

    struct Drone *da = (struct Drone *)a;
    struct Drone *db = (struct Drone *)b;

    if (da->x < db->x)
        return -1;

    if (da->x > db->x)
        return 1;

    return 0;
}

// Fonction récursive Divide and Conquer
float closest_pair_rec(struct Drone *essaim,
                       int n,
                       int *id1,
                       int *id2) {

    // Cas simple : petit nombre de drones
    if (n <= 8) {

        float min_d = FLT_MAX;

        for (int i = 0; i < n; i++) {

            for (int j = i + 1; j < n; j++) {

                float d =
                    distance_sq(essaim + i,
                                essaim + j);

                // Mise à jour de la distance minimale
                if (d < min_d) {

                    min_d = d;

                    *id1 = (essaim + i)->id;
                    *id2 = (essaim + j)->id;
                }
            }
        }

        return min_d;
    }

    // Division du tableau en deux parties
    int mid = n / 2;

    float mid_x = (essaim + mid)->x;

    int lid1, lid2;
    int rid1, rid2;

    // Recherche dans la partie gauche
    float d_left =
        closest_pair_rec(essaim,
                         mid,
                         &lid1,
                         &lid2);

    // Recherche dans la partie droite
    float d_right =
        closest_pair_rec(essaim + mid,
                         n - mid,
                         &rid1,
                         &rid2);

    float d_min;

    // Conservation de la meilleure distance
    if (d_left < d_right) {

        d_min = d_left;

        *id1 = lid1;
        *id2 = lid2;
    }
    else {

        d_min = d_right;

        *id1 = rid1;
        *id2 = rid2;
    }

    // Largeur de la bande centrale
    float strip_d = sqrtf(d_min);

    // Allocation mémoire pour la bande
    struct Drone *strip =
        (struct Drone *)malloc(n * sizeof(struct Drone));

    if (!strip) {

        fprintf(stderr,
                "Erreur malloc strip\n");

        exit(1);
    }

    int strip_size = 0;

    // Construction de la bande centrale
    for (int i = 0; i < n; i++) {

        float dx =
            (essaim + i)->x - mid_x;

        if (dx < 0)
            dx = -dx;

        if (dx < strip_d) {

            *(strip + strip_size) =
                *(essaim + i);

            strip_size++;
        }
    }

    // Vérification des drones dans la bande
    for (int i = 0; i < strip_size; i++) {

        for (int j = i + 1; j < strip_size; j++) {

            float dy =
                (strip + j)->y -
                (strip + i)->y;

            if (dy >= strip_d)
                break;

            float d =
                distance_sq(strip + i,
                            strip + j);

            // Nouvelle distance minimale trouvée
            if (d < d_min) {

                d_min = d;

                strip_d = sqrtf(d_min);

                *id1 = (strip + i)->id;
                *id2 = (strip + j)->id;
            }
        }
    }

    // Libération mémoire
    free(strip);

    return d_min;
}

int main(void) {

    // Seed aléatoire différente à chaque exécution
    srand(time(NULL));

    // Allocation dynamique des drones
    struct Drone *essaim =
        (struct Drone *)malloc(N * sizeof(struct Drone));

    if (!essaim) {

        fprintf(stderr,
                "Allocation memoire echouee\n");

        return 1;
    }

    // Génération aléatoire des coordonnées
    for (int i = 0; i < N; i++) {

        (essaim + i)->id = i + 1;

        (essaim + i)->x =
            ((float)rand() / RAND_MAX) * 10000.0f;

        (essaim + i)->y =
            ((float)rand() / RAND_MAX) * 10000.0f;

        (essaim + i)->z =
            ((float)rand() / RAND_MAX) * 10000.0f;
    }

    // Tri des drones selon x
    qsort(essaim,
          N,
          sizeof(struct Drone),
          compare_x);

    int id1 = -1;
    int id2 = -1;

    // Début mesure du temps
    clock_t start = clock();

    // Recherche de la paire la plus proche
    float dist_sq =
        closest_pair_rec(essaim,
                         N,
                         &id1,
                         &id2);

    // Fin mesure du temps
    clock_t end = clock();

    float dist = sqrtf(dist_sq);

    double elapsed =
        (double)(end - start)
        / CLOCKS_PER_SEC * 1000.0;

    // Affichage des résultats
    printf("\n===== RESULTAT =====\n");

    printf("Paire la plus proche : ");
    printf("Drone %d et Drone %d\n",
           id1,
           id2);

    printf("Distance euclidienne : ");
    printf("%.4f m\n", dist);

    printf("Temps d execution : ");
    printf("%.3f ms\n", elapsed);

    // Libération mémoire
    free(essaim);

    return 0;
}
