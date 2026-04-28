#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Hello World");
    return 0;
}


// --- Dans include/carte.h ---

// Les types de terrains demandés
typedef enum { 
    PLAINE, FORET, MONTAGNE, EAU, DESERT, TOUNDRA 
} TypeTerrain;

// Une case de la carte
typedef struct {
    TypeTerrain terrain;
    struct Unite* unite_presente; // Pointeur vers l'unité sur la case (NULL si vide)
    struct Ville* ville_presente; // Pointeur vers la ville sur la case (NULL si vide)
} Tuile;

// La carte complète
typedef struct {
    int largeur;
    int hauteur;
    Tuile** grille; // Tableau 2D alloué dynamiquement
} Carte;

// --- Dans include/entites.h ---

typedef enum { COLON, GUERRIER } TypeUnite;

typedef struct Unite {
    TypeUnite type;
    int pv_courants;
    int pv_max;
    int attaque;
    int defense;
    int pm_restants;
    int pm_max;
    int posX; // Position sur la carte
    int posY;
} Unite;

typedef struct Ville {
    int population;
    int nourriture_stockee;
    int pv_courants;
    int pv_max;
    int posX;
    int posY;
    // Tu pourras ajouter le projet en cours plus tard
} Ville;