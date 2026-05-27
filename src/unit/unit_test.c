#include "unit.h"
#include "../map/map.h"
#include "../tile/tile.h"
#include "../game/game.h"
#include "../configuration/configuration.h"
#include "../barbarian/barbarian.h"
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
    assert(get_pv(colon) == 10);       
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

    // 5. Test du combat entre une unité du joueur et un barbare
    Configuration* config = create_configuration(30, 20, 1000, 30, 2, 0);
    Game* game = create_game(config);

    Position warrior_pos = {5, 5};
    Position barb_pos = {6, 5};

    Unit* warrior = create_unit('g', warrior_pos);
    Barbarian* barb = create_barbarian(barb_pos);

    Tile* warrior_tile = get_tile(game->map, warrior_pos);
    Tile* barb_tile = get_tile(game->map, barb_pos);

    assert(warrior_tile != NULL);
    assert(barb_tile != NULL);

    /*
     * On force deux cases terrestres pour tester uniquement la logique de combat,
     * sans dépendre du biome généré sur la carte.
     */
    warrior_tile->biome = 'P';
    barb_tile->biome = 'P';

    /*
     * Les unités du joueur sont stockées dans les tuiles,
     * tandis que les barbares sont aussi référencés dans leur propre liste.
     */
    warrior_tile->unit = warrior;
    barb_tile->barb_on = barb;

    UnitList* game_unit_list = malloc(sizeof(UnitList));
    assert(game_unit_list != NULL);
    game_unit_list->data = warrior;
    game_unit_list->next = NULL;
    game->unitList = game_unit_list;

    BarbarianList* game_barb_list = create_barb_list(barb, NULL);
    game->barbarianList = game_barb_list;
    game->barbs_number = 1;

    int warrior_pv_before = warrior->pv;
    int barb_pv_before = barb->pv;

    MoveResult result = move_unit_step(game, warrior, barb_pos);

    /*
     * Le déplacement vers une case occupée par un barbare doit déclencher un combat,
     * pas être traité comme un déplacement classique.
     */
    assert(result == MOVE_COMBAT_WIN ||
           result == MOVE_COMBAT_BLOCKED ||
           result == MOVE_ATTACKER_DEAD);

    if (result == MOVE_COMBAT_BLOCKED) {
        assert(warrior->pos.x == warrior_pos.x);
        assert(warrior->pos.y == warrior_pos.y);
        assert(warrior->pv < warrior_pv_before || barb->pv < barb_pv_before);
    }

    if (result == MOVE_COMBAT_WIN) {
        assert(warrior->pos.x == barb_pos.x);
        assert(warrior->pos.y == barb_pos.y);
        assert(barb_tile->unit == warrior);
        assert(barb_tile->barb_on == NULL);
    }

    printf("-> Test Combat Unite contre Barbare : OK\n");

    // Nettoyage
    free(ma_liste->next);
    free(ma_liste);
    destroy_unit(colon);
    destroy_unit(guerrier);
    destroy_game(game);

    printf("=== TOUS LES TESTS UNIT PASSENT AVEC SUCCÈS ! ===\n");

    return 0;
}
