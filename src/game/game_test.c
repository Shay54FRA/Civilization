#include "game.h"
#include "../map/map.h"
#include "../tile/tile.h"
#include "../configuration/configuration.h"
#include "../city/city.h"
#include "../cli/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main(void){
    assert(strcmp(get_name('C'), "Caserne") == 0);
    assert(get_cost('C') == 60);
    assert(get_entretien_cost('C') == 2);
    assert(get_entretien_cost('G') == 1);
    assert(get_entretien_cost('Z') == -1);

    Configuration* config = create_configuration(20, 30, 1000, 100, 3, 0);
    Game* game = create_game(config);

    print_map_cli(game->map, game->starting_point);
    
    give_all_bonuses(game);
    printf("Gold : %d\n", game->gold);
    printf("Science : %d\n", game->science);
    printf("Food : %d\n", game->cityList->city->food);
    printf("Prod : %d\n", game->cityList->city->production);

    destroy_game(game);

    return 0;
}
