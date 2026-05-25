#include "barbarian.h"
#include "../game/game.h"
#include "../configuration/configuration.h"
#include "../unit/unit.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    Configuration* config = create_configuration(30, 20, 1000, 30, 2, 0);
    Game* game = create_game(config);
    Position pos = {4,5};
    Barbarian* barb = create_barbarian(pos);
    BarbarianList* barb_list = create_barb_list(barb, NULL);
    game->barbarianList = barb_list;
    Position pos_bis = {10,15};
    Unit* unit = create_unit('c', pos_bis);
    UnitList* unit_list = malloc(sizeof(UnitList));
    unit_list->data = unit;
    unit_list->next = NULL;
    game->unitList = unit_list;
    Position target = get_nearest_target(game, barb);
    printf("Position de la cible : %d : %d\n", target.x, target.y);
    printf("Avant mouvement du barbare : %d : %d\n", barb->pos->x, barb->pos->y);
    move_all_barbarians(game);
    printf("Après mouvement du barbare : %d : %d\n", barb->pos->x, barb->pos->y);

    destroy_game(game);
}
