#include "map.h"
#include "../tile/tile.h"
#include "../cli/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// --- INTERRUPTEURS POUR LES TESTS ---
// Mets à 1 pour activer le module, 0 pour le désactiver
#define ACTIVER_TEST_TECH 1 
#define ACTIVER_TEST_UNIT 1 

#if ACTIVER_TEST_TECH
#include "../technology/technology.h"
#endif

#if ACTIVER_TEST_UNIT
#include "../unit/unit.h"
#endif


int main(void) {
    
    Map* ma_carte = create_map(20, 15, 42); 
    Position curseur = {0, 0}; 
    char input = ' ';

    // --- INITIALISATION DE LA PARTIE DE TEST (Commune) ---
    Game dummy_game;
    dummy_game.map = ma_carte; // INDISPENSABLE pour les déplacements et autres
    dummy_game.science = 100; 
    dummy_game.active_research_id = -1; 

#if ACTIVER_TEST_TECH
    dummy_game.tech_tree = create_tech_tree(); 
#endif

#if ACTIVER_TEST_UNIT
    // --- CREATION D'UNE UNITE DE TEST ---
    Position pos_unite = {2, 2};
    Unit* test_unit = create_unit('g', pos_unite); // On crée un Guerrier
    get_tile(ma_carte, pos_unite)->unit = test_unit; // On le pose sur la carte
    Unit* selected_unit = NULL; // Mémoire pour l'unité qu'on est en train de bouger
#endif

    // --- LA BOUCLE DE JEU ---
    while (input != 'p') { 
        
        print_map_cli(ma_carte, curseur);
        
        printf("LÉGENDE : ");
        printf("%s EAU %s ", BG_EAU, COLOR_RESET);
        printf("%s PLAINE %s ", BG_PLAINE, COLOR_RESET);
        printf("%s FORÊT %s ", BG_FORET, COLOR_RESET);
        printf("%s MONTAGNE %s ", BG_MONTAGNE, COLOR_RESET);
        printf("%s DÉSERT %s ", BG_DESERT, COLOR_RESET);
        printf("%s TOUNDRA %s\n", BG_TOUNDRA, COLOR_RESET);
        
        printf("Navigation : z (Haut), s (Bas), q (Gauche), d (Droite), p (Quitter)\n");
        
#if ACTIVER_TEST_UNIT
        printf("Action Unite : m (Saisir / Poser une unite)\n"); 
        if (selected_unit != NULL) {
            printf("\n>>> Unite [%c] selectionnee ! Deplacez le curseur et appuyez sur 'm' pour confirmer le deplacement. <<<\n", selected_unit->type);
            printf(">>> PM restants : %d / %d <<<\n", selected_unit->pm, selected_unit->max_pm);
        }
#endif

#if ACTIVER_TEST_TECH
        printf("Action Tech  : t (Ouvrir l'Arbre des Technologies)\n");
#endif

        printf("Action : ");
        scanf(" %c", &input); 

        // --- GESTION DU CURSEUR ---
        if (input == 'z' && curseur.y > 0) curseur.y--;
        if (input == 's' && curseur.y < ma_carte->height - 1) curseur.y++;
        if (input == 'q' && curseur.x > 0) curseur.x--;
        if (input == 'd' && curseur.x < ma_carte->length - 1) curseur.x++;

        // --- GESTION DES UNITES ---
#if ACTIVER_TEST_UNIT
        if (input == 'm') {
            if (selected_unit == NULL) {
                // 1. Essayer d'attraper une unité
                Tile* t = get_tile(ma_carte, curseur);
                if (t != NULL && t->unit != NULL) {
                    selected_unit = t->unit;
                    printf("\n-> SUCCESS : Vous avez saisi l'unite '%c' !\n", selected_unit->type);
                } else {
                    printf("\n-> ERREUR : Aucune unite sous le curseur.\n");
                }
            } else {
                // 2. Essayer de déposer l'unité qu'on a en main
                bool success = move_unit_step(&dummy_game, selected_unit, curseur);
                if (success) {
                    printf("\n-> SUCCESS : Deplacement effectue !\n");
                } else {
                    printf("\n-> ERREUR : Deplacement impossible sur cette case.\n");
                }
                // On lâche l'unité quoiqu'il arrive pour éviter de la garder "collée"
                selected_unit = NULL; 
            }
        }
#endif

        // --- GESTION DE LA TECHNOLOGIE ---
#if ACTIVER_TEST_TECH
        update_research(&dummy_game);
        if (input == 't') {
            show_technology_menu(&dummy_game);
        }
#endif
        printf("\n"); 
    }

    // --- NETTOYAGE (Important pour éviter les fuites de mémoire) ---
#if ACTIVER_TEST_TECH
    destroy_tech_tree(dummy_game.tech_tree);
#endif

#if ACTIVER_TEST_UNIT
    destroy_unit(test_unit);
#endif

    destroy_map(ma_carte);

    return 0;
}