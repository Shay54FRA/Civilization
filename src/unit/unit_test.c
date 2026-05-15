#include "unit.h"
#include "../map/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main(void) {
    printf("=== Lancement des tests unitaires : MODULE UNIT ===\n");

    // 1. Test de création d'un Colon ('c')
    Position pos_c = {5, 5};
    Unit* colon = create_unit('c', pos_c);
    
    assert(colon != NULL);
    assert(get_unit_type(colon) == 'c');
    assert(get_pv(colon) == 1);       
    assert(get_atk(colon) == 0);      
    assert(get_pm(colon) == 2);       
    assert(get_unit_pos(colon).x == 5);
    printf("-> Test Colon : OK\n");

    // 2. Test de création d'un Guerrier ('g')
    Position pos_g = {10, 12};
    Unit* guerrier = create_unit('g', pos_g);
    
    assert(guerrier != NULL);
    assert(get_unit_type(guerrier) == 'g');
    assert(get_pv(guerrier) == 15);   
    assert(get_atk(guerrier) == 3);   
    assert(get_pm(guerrier) == 3);    
    assert(get_cost_per_turn(guerrier) == 1); 
    printf("-> Test Guerrier : OK\n");

    // 3. Test de la liste chaînée (UnitList)
    UnitList* ma_liste = malloc(sizeof(UnitList));
    ma_liste->data = guerrier;
    ma_liste->next = malloc(sizeof(UnitList));
    ma_liste->next->data = colon;
    ma_liste->next->next = NULL;

    assert(get_unit(ma_liste) == guerrier);
    assert(get_unit(get_unit_next(ma_liste)) == colon);
    printf("-> Test UnitList (Logique de liste) : OK\n");

    // 4. Test des PM (Points de Mouvement)
    guerrier->pm -= 1; 
    assert(get_pm(guerrier) == 2);
    reset_all_pm(ma_liste); // On teste la fonction de remise à zéro du tour
    assert(get_pm(guerrier) == 3);
    printf("-> Test Reset PM : OK\n");

    // Nettoyage
    free(ma_liste->next);
    free(ma_liste);
    destroy_unit(colon);
    destroy_unit(guerrier);

    printf("=== TOUS LES TESTS UNIT PASSENT AVEC SUCCÈS ! ===\n");

    return 0;
}
