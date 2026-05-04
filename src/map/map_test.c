#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// --- INTERRUPTEUR POUR TES TESTS ---
// Mets 1 pour activer le menu avec 't' (technologies), ou 0 pour désactiver !
#define ACTIVER_TEST_TECH 0 

#if ACTIVER_TEST_TECH
#include "../game/game.h"
#include "../technology/technology.h"
#endif

int main(void) {
    Map* ma_carte = create_map(20, 15, 42); 
    
    Position curseur = {0, 0}; // On commence en haut à gauche
    char input = ' ';

#if ACTIVER_TEST_TECH
    Game dummy_game;
    dummy_game.science = 100; // Un peu de science pour tester les achats
    dummy_game.tech_tree = create_tech_tree(); 
#endif

    // La boucle de jeu
    while (input != 'p') { // 'p' pour quitter la map
        
        print_map(ma_carte, curseur);
        
        printf("LÉGENDE : ");
        printf("%s EAU %s ", BG_EAU, COLOR_RESET);
        printf("%s PLAINE %s ", BG_PLAINE, COLOR_RESET);
        printf("%s FORÊT %s ", BG_FORET, COLOR_RESET);
        printf("%s MONTAGNE %s ", BG_MONTAGNE, COLOR_RESET);
        printf("%s DÉSERT %s ", BG_DESERT, COLOR_RESET);
        printf("%s TOUNDRA %s\n", BG_TOUNDRA, COLOR_RESET);
        printf("Navigation : z (Haut), s (Bas), q (Gauche), d (Droite), p (Quitter)\n");
        
#if ACTIVER_TEST_TECH
        printf("Action supplementaire : t (Ouvrir l'Arbre des Technologies)\n");
#endif
        printf("Action :");
        
        scanf(" %c", &input); 

        // Mise à jour du curseur (en faisant attention à ne pas sortir de la map)
        if (input == 'z' && curseur.y > 0) curseur.y--;
        if (input == 's' && curseur.y < ma_carte->height - 1) curseur.y++;
        if (input == 'q' && curseur.x > 0) curseur.x--;
        if (input == 'd' && curseur.x < ma_carte->length - 1) curseur.x++;

#if ACTIVER_TEST_TECH
        if (input == 't') {
            show_technology_menu(&dummy_game);
        }
#endif

        printf("\n"); // Si on retourne pas à la ligne, on a un bug graphique sur la 1ère ligne de cases
    }

    // Tile* ma_tuile = get_tile(ma_carte, pos);
    // print_tile(ma_tuile);
    // TileList* tuiles_voisines = get_neighbors(ma_carte, ma_tuile);
    // print_tilelist(tuiles_voisines);

    // destroy_tilelist(tuiles_voisines);

#if ACTIVER_TEST_TECH
    destroy_tech_tree(dummy_game.tech_tree);
#endif

    destroy_map(ma_carte);
    return 0;
}
