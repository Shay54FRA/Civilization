#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "technology.h"
#include "../game/game.h"

int main(void) {
    // 1. Initialisation de l'arbre et d'une partie de test
    TechTree* tree = create_tech_tree();
    assert(tree != NULL); 

    Game test_game = {0};
    test_game.science = 0; 
    test_game.active_research_id = -1; // Aucun projet en cours au départ
    test_game.tech_tree = tree;
    test_game.unitList = NULL;

    // 2. Test des prérequis
    assert(can_research_tech(&test_game, tree, 2) == 1); // L'Agriculture (ID 2) nécessite le Départ (déjà débloqué)
    assert(can_research_tech(&test_game, tree, 6) == 0); // L'Irrigation (ID 6) nécessite l'Agriculture

    // 3. Lancement d'un projet de recherche
    set_active_research(&test_game, 2); // On lance l'Agriculture
    assert(test_game.active_research_id == 2); 

    // --- Test de la pénalité de changement de projet ---
    test_game.science = 40; 
    set_active_research(&test_game, 1); // On change d'avis pour la Chasse (ID 1)
    assert(test_game.science == 20);    // La science doit être divisée par 2 !
    assert(test_game.active_research_id == 1);

    // 4. Simulation des tours (accumulation de science)
    test_game.science = 30; 
    update_research(&test_game); 
    
    assert(tree->technologies[1].is_unlocked == 0); // Coût de 50 (Chasse) non atteint
    assert(test_game.active_research_id == 1); 

    test_game.science = 65; // On dépasse les 50 requis
    update_research(&test_game); 
    
    // 5. Vérification du déblocage et des règles du CDC
    assert(tree->technologies[1].is_unlocked == 1); // Découverte validée
    assert(test_game.active_research_id == -1);     // Le post-it est vidé
    assert(test_game.science == 0);                 // Le surplus de science est perdu

    // Vérification de l'application des bonus (Chasse donne +1 sur Forêt)
    assert(tree->technologies[1].bonus.bonus_food_forest == 1);
    
    // On triche en lançant et terminant l'Artisanat (ID 3) pour tester le déblocage d'unité
    set_active_research(&test_game, 3);
    test_game.science = 70;
    update_research(&test_game);
    
    // L'Artisanat débloque le Guerrier ('g'), vérifions que le tableau de Nathan marche :
    assert(is_unit_unlocked(tree, 'g') == 1);
    // Mais on n'a pas encore l'Écriture, donc la Bibliothèque ('B') doit rester bloquée :
    assert(is_building_unlocked(tree, 'B') == 0);

    // Test Equitation (ID 5) pour les PM
    set_active_research(&test_game, 5);
    test_game.science = 100;
    update_research(&test_game);
    assert(tree->technologies[5].is_unlocked == 1);
    assert(tree->bonus_pm_units == 1);
    
    printf("-> Tous les tests unitaire (y compris penalite et optimisation) ont REUSSI !\n");

    // 6. Nettoyage de la mémoire
    destroy_tech_tree(tree);

    return 0;
}
