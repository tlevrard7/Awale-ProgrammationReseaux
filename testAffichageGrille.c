#include <stdio.h>

#define LARGEUR 6
#define LONGUEUR 2

// Struct pour représenter le tableau 2D de 2x6 cases
typedef struct {
    int cases[LONGUEUR][LARGEUR];
} Grille;

void afficherGrille(Grille *grille) {
    for (int i = 0; i < LONGUEUR; i++) {
        // Ligne supérieure de chaque cellule
        for (int j = 0; j < LARGEUR; j++) {
            printf("+--");
        }
        printf("+\n");

        // Affichage des valeurs dans chaque cellule
        for (int j = 0; j < LARGEUR; j++) {
            printf("|%2d", grille->cases[i][j]);
        }
        printf("|\n");
    }

    // Ligne du bas de chaque cellule
    for (int j = 0; j < LARGEUR; j++) {
        printf("+--");
    }
    printf("+\n");
}

int main() {
    Grille maGrille;

    // Affichage de la grille
    afficherGrille(&maGrille);

    return 0;
}
