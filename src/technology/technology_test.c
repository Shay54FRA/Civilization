#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "technology.h"
#include "../game/game.h"

int main() {
    //printf("\n=== DEBUT DES TESTS TECHNOLOGY ===\n");

    // 1. Initialisation de l'arbre
    TechTree* tree = create_tech_tree();
    assert(tree != NULL); 
    //printf("- Arbre de technologie cree avec succes.\n");

    // 2. Création d'une fausse partie "Game" juste pour le test
    Game test_game;
    test_game.science = 15; // On se donne 15 de science

    // 3. Test des conditions
    //printf("- Verification des conditions de deblocage...\n");
    // On devrait pouvoir débloquer l'Agriculture
    assert(can_unlock_tech(&test_game, tree, 0) == 1); 
    
    // On ne devrait PAS pouvoir débloquer l'Elevage (manque de science + manque prérequis)
    assert(can_unlock_tech(&test_game, tree, 1) == 0); 

    // ... (le début du test reste pareil) ...

    // 4. Test de l'achat
    //printf("- Achat de l'Agriculture...\n");
    unlock_tech(&test_game, tree, 0);
    
    assert(test_game.science == 5); 
    assert(tree->technologies[0].is_unlocked == 1);
    //printf("- Succes : L'Agriculture est debloquee et la science a ete debitee !\n");

    // --- NOUVEAU : Test des fonctions de vérification ---
    //printf("- Verification des batiments et unites debloques...\n");
    
    // L'agriculture débloque le Grenier ('G'), donc ça doit être vrai (1)
    assert(is_building_unlocked(tree, 'G') == 1);
    
    // L'agriculture ne débloque pas la Muraille ('R'), donc ça doit être faux (0)
    assert(is_building_unlocked(tree, 'R') == 0);
    
    // L'Elevage n'est pas encore acheté, donc le Cavalier ('C') doit être bloqué (0)
    assert(is_unit_unlocked(tree, 'C') == 0);
    
    //printf("- Succes : Les ponts avec le reste du jeu fonctionnent parfaitement !\n");
    // ----------------------------------------------------

    // 5. Nettoyage de la mémoire
    destroy_tech_tree(tree);
    //printf("=== TESTS TECHNOLOGY TERMINES AVEC SUCCES ===\n\n");

    return 0;
}
