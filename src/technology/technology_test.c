#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "technology.h"
#include "../game/game.h"

int main(void) {
    // 1. Initialisation de l'arbre et d'une partie de test
    TechTree* tree = create_tech_tree();
    assert(tree != NULL); 

    Game test_game;
    test_game.science = 0; 
    test_game.active_research_id = -1; // Aucun projet en cours au départ
    test_game.tech_tree = tree;

    // 2. Test des prérequis
    assert(can_research_tech(&test_game, tree, 0) == 1); // L'Agriculture n'a pas de prérequis
    assert(can_research_tech(&test_game, tree, 1) == 0); // L'Elevage nécessite l'Agriculture

    // 3. Lancement d'un projet de recherche
    set_active_research(&test_game, 0); 
    assert(test_game.active_research_id == 0); 

    // 4. Simulation des tours (accumulation de science)
    test_game.science = 30; 
    update_research(&test_game); 
    
    assert(tree->technologies[0].is_unlocked == 0); // Coût de 60 non atteint
    assert(test_game.active_research_id == 0); 

    test_game.science = 65; 
    update_research(&test_game); 
    
    // 5. Vérification du déblocage et des règles du CDC
    assert(tree->technologies[0].is_unlocked == 1); // Découverte validée
    assert(test_game.active_research_id == -1);     // Le post-it est vidé
    assert(test_game.science == 0);                 // Le surplus de science est perdu

    // Vérification de l'application des bonus
    assert(tree->technologies[0].bonus.bonus_food_percent == 10);
    assert(is_unit_unlocked(tree, 'C') == 0);
    
    // 6. Nettoyage de la mémoire
    destroy_tech_tree(tree);

    return 0;
}
