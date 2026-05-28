#include "game.h"
#include "../building/building.h"
#include "../map/map.h"
#include "../tile/tile.h"
#include "../city/city.h"
#include "../unit/unit.h"
#include "../technology/technology.h"
#include "../configuration/configuration.h"
#include "../barbarian/barbarian.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

TupleRessources* create_tuple_ressources(void) {
    TupleRessources* rep = malloc(sizeof(TupleRessources));
    if (rep != NULL) {
        rep->ressource1 = 0;
        rep->ressource2 = 0;
    }
    return rep;
}

void destroy_tuple_ressources(TupleRessources* structure) {
    if (structure != NULL) {
        free(structure);
    }
}

Game* create_game(Configuration* config) {
    if (config != NULL) {
        Game* game = malloc(sizeof(Game));
        if (game != NULL) {
            game->science = 0;
            game->gold = 0;
            game->active_research_id = -1;
            game->active_turn = 1;
            game->configuration = config;
            game->barbs_number = 0;
            game->map = create_map(get_width(config), get_height(config), get_seed(config), get_nbr_camps_barbares(config));
            Position pos = get_starting_city_pos(game->map);
            City* base_city = create_city(pos);
            game->barbarianList = NULL; //Pas de barbares au départ
            game->starting_point = pos;
            game->poverty = false;
            game->turns_10_cities = 0;
            game->turns_no_productions = 0;
            game->cityList = NULL; //S'assurer que l'espace est libre
            create_city_list(game, base_city); 

            Tile* tile_to_use = get_tile(game->map, pos);
            tile_to_use->city_on = true;

            game->unitList = NULL; //Pas d'unités au départ
            game->tech_tree = create_tech_tree();
            game->new_ressources = create_tuple_ressources();
            return game;
        }
    }
    return NULL;
}

void destroy_game(Game* game) {
    if (game != NULL) {
        destroy_tech_tree(game->tech_tree);
        destroy_tuple_ressources(game->new_ressources);
        destroy_unit_list(game->unitList);
        destroy_barb_list(game->barbarianList);
        destroy_city_list(game->cityList);
        destroy_map(game->map);
        destroy_configuration(game->configuration);
        free(game);
    }
}

char* get_name(char type){
    if (type == 'G') return "Grenier";
    if (type == 'M') return "Marché";
    if (type == 'A') return "Atelier";
    if (type == 'B') return "Bibliothèque";
    if (type == 'C') return "Caserne";
    if (type == 'R') return "Muraille";
    if (type == 'c') return "Colon";
    if (type == 'g') return "Guerrier";
    return NULL;
}

int get_cost(char type){
    if (type == 'G') return 30;
    if (type == 'M') return 40;
    if (type == 'A') return 40;
    if (type == 'B') return 50;
    if (type == 'C') return 60;
    if (type == 'R') return 80;
    if (type == 'c') return 50;
    if (type == 'g') return 40;
    return -1;
}

int get_entretien_cost(char type) {
    if (type == 'c') {
        return 0;
    }
    if (type == 'G' || type == 'M' || type == 'A' || type == 'B' || type == 'c') {
        return 1;
    }
    if (type == 'C' || type == 'R') {
        return 2;
    }
    return -1; 
}

int get_all_entretien_costs(Game* game) {
    if (game == NULL) return -1;
    int rep = get_total_unit_maintenance(game);
    CityList* city_to_check = game->cityList;
    City* city;

    while (city_to_check != NULL) {
        city = city_to_check->city;
        if (city != NULL) {
            BuildList* to_check = city->buildings;
            while (to_check != NULL) {
                Building* build = to_check->data;
                if (build != NULL) {
                    rep += get_entretien_cost(build->type);
                }
                to_check = to_check->next;
            }
        }
        city_to_check = city_to_check->next;
    }
    return rep;
}

int get_city_number(Game* game) {
    if (game != NULL) {
        int rep = 0;
        CityList* to_check = game->cityList;
        while (to_check != NULL) {
            if (to_check->city != NULL) rep += 1;
            to_check = to_check->next;
        }
        return rep;
    }
    return -1;
}

int get_camp_list_number(Game* game) {
    int rep = 0;
    Tile* tile;
    for (int x = 0; x < game->map->length; x++) {
        for (int y = 0; y < game->map->height; y++) {
            Position pos = {x, y};
            tile = get_tile(game->map, pos);
            if (tile->camp_on) rep += 1;
        }
    }
    return rep;
}

static void remove_unit_from_game(Game* game, Unit* unit) {
    if (game == NULL || unit == NULL) return;

    UnitList* current = game->unitList;
    UnitList* previous = NULL;

    while (current != NULL) {
        if (current->data == unit) {
            if (previous == NULL) {
                game->unitList = current->next;
            }
            else {
                previous->next = current->next;
            }

            free(current);
            return;
        }

        previous = current;
        current = current->next;
    }
}

void colonize(Game* game, Unit* colon) {
    if (game == NULL || game->map == NULL || colon == NULL) return;
    if (colon->type != 'c') return;

    Tile* tile = get_tile(game->map, colon->pos);
    if (tile == NULL) return;
    if (tile->city_on) return;

    City* city = create_city(colon->pos);
    if (city == NULL) return;

    CityList* node = malloc(sizeof(CityList));
    if (node == NULL) {
        free(city);
        return;
    }

    node->city = city;
    node->next = game->cityList;
    game->cityList = node;

    tile->city_on = true;
    tile->unit = NULL;

    remove_unit_from_game(game, colon);
    destroy_unit(colon);
}

int get_unit_number(Game* game) {
    if (game != NULL) {
        int rep = 0;
        UnitList* to_check = game->unitList;
        while (to_check != NULL) {
            if (to_check->data != NULL) {
                rep += 1;
            }
            to_check = to_check->next;
        }
        return rep;
    }
    return -1;
}

int get_new_gold(Game* game) {
    if (game == NULL) return -1;
    if (game->new_ressources == NULL) return -1;
    return game->new_ressources->ressource1;
}
int get_new_science(Game* game) {
    if (game == NULL) return -1;
    if (game->new_ressources == NULL) return -1;
    return game->new_ressources->ressource2;
}

//Renvoie un booléen pour connaître s'il y a pauvreté ou non
bool check_poor(Game* game) {
    if (game == NULL) return false;
    if (game->gold < 0) {
        int unit_number = get_unit_number(game);
        if (unit_number == 0) {
            game->poverty = true;
        } else {
            game->poverty = false;
            int random_destroy_number = (rand() % unit_number);
            kill_nth_unit(game, random_destroy_number);
        }
        game->gold = 0;
        return true;
    }
    game->poverty = false;
    return false;
}

int end_turn(Game* game) {
    if (game == NULL) return 0;
    // PHASE DE PRODUCTION
    give_all_bonuses(game);
    update_research(game);
    update_city_projects(game);
    game->gold -= get_all_entretien_costs(game);
    update_food(game->cityList);

    // PHASE DE CROISSANCE
    check_poor(game);
    update_croissance(game);

    // PHASE DE BARBARES
    move_all_barbarians(game);
    spawn_all_barbarians(game);

    //PHASE DE FIN DE TOUR
    heal_cities(game->cityList);
    reset_exploitation(game->map);
    reset_all_barbs_pm(game->barbarianList);
    reset_all_pm(game->unitList);
    if (get_city_number(game) >= 10) {
        game->turns_10_cities ++;
    } else {
        game->turns_10_cities = 0;
    }
    return end_game(game);
}

int game_score(Game* game) {
    int tech_number = game->tech_tree->num_unlocked;
    int total_pop = get_total_pop(game->cityList);
    int city_number = get_city_number(game);
    int bonus_turn = (game->configuration->t - game->active_turn);
    if (bonus_turn < 0) bonus_turn = 0;
    return 100 * tech_number + 20 * total_pop + 50 * city_number + 10 * bonus_turn;
}

int end_game(Game* game) {
    if (game == NULL) return 0;
    if (game->configuration == NULL || game->tech_tree == NULL) return 0;

    if (get_city_number(game) == 0) return 3; //Défaite aucune ville
    if (game->active_turn >= game->configuration->t) return 3; //Défaite nombre de tour max atteint
    /* A compléter : défaite si prod nulle pdt 5 tours de suite */

    if (game->turns_10_cities >= 5) return 1; //Victoire territoriale
    if (game->tech_tree->num_unlocked == (game->tech_tree->num_technologies-1)) return 2; //Victoire technologique

    return 0; //Partie non terminée
}

void give_bonus_building(Game* game, City* city, Building* building) {
    switch (building->type) {
        case 'G': city->new_ressources->ressource1 += 3; break; //Food

        case 'A': city->new_ressources->ressource2 += 3; break; //Prod

        case 'B': game->new_ressources->ressource2 += 4; break; //Science

        case 'M': game->new_ressources->ressource1 += 3; break; //Gold

        /* Les 2 autres batiments ne donnent que des bonus au lancement */
    }
}

void give_bonus_tile(Game* game, City* city, Tile* tile) {
    if (game != NULL && city != NULL && tile != NULL) {
        switch(tile->biome) {
            case 'P': city->new_ressources->ressource1 += 2;        //Food
                city->new_ressources->ressource2 += 1; break;       //Prod

            case 'E': city->new_ressources->ressource1 += 1;        //Food
                game->new_ressources->ressource1 += 1; break;       //Gold

            case 'M': city->new_ressources->ressource2 += 3;        //Prod
                game->new_ressources->ressource2 += 1; break;       //Science

            case 'F': city->new_ressources->ressource1 += (1 + game->tech_tree->bonus_food_forest);        //Food
                city->new_ressources->ressource2 += 2; break;       //Prod

            case 'T': city->new_ressources->ressource1 += 1;        //Food
                city->new_ressources->ressource2 += 1; break;       //Prod

            case 'D': game->new_ressources->ressource1 += 1; break; //Gold

        }
    }
}

TileList* get_exploitation_range(Game* game, City* city, int range) {
    if (city != NULL) {
        Position pos = city->pos;
        Tile* tile = get_tile(game->map, pos);
        TileList* exploit_of_building = get_exploited_tiles(game->map, tile, range);
        mark_exploited_tiles(exploit_of_building);
        return exploit_of_building;
    }
    return NULL;
} 

TileList** get_all_exploited_tiles(Game* game) {
    if (game != NULL) {
        int len_city = get_city_number(game);
        TileList** listeTilelist = malloc(len_city*sizeof(TileList*));
        for (int i = 0; i <len_city; i++) {
            listeTilelist[i] = create_tilelist(NULL);
        }
        CityList* to_check;
        City* city;
        for (int range = 0; range < 4; range++) {
            to_check = game->cityList;
            for (int ind_city = 0; ind_city < len_city; ind_city++) {
                city = to_check->city;
                if (range <= EXPLOITATION_RANGE) {
                    merge_and_destroy_tilelists(listeTilelist[ind_city], get_exploitation_range(game, city, range));
                }
                to_check = to_check->next;
            }
        }
        return listeTilelist;
    }
}

void destroy_all_tilelists(TileList** tabTilelist, int length) {
    if (tabTilelist != NULL) {
        for (int i = 0; i < length; i++) {
            destroy_tilelist(tabTilelist[i]);
        }
        free(tabTilelist);
    }
}

void give_bonus_city_buildings(Game* game, City* city) {
    if (city != NULL) {
        BuildList* to_check = city->buildings;
        while (to_check != NULL) {
            Building* build = to_check->data;
            give_bonus_building(game, city, build);
            to_check = to_check->next;
        }
    }
}

void give_bonus_city_exploited_tiles(Game* game, City* city, TileList* exploitList) {
    TileList* to_check = exploitList;
    if (game != NULL && city != NULL) {
        while (to_check != NULL) {
            Tile* tile = to_check->data;
            give_bonus_tile(game, city, tile);
            to_check = to_check->next;
        }
    }
}

void give_bonus_exploitations(Game* game, TileList** tab) {
    if (game != NULL && tab != NULL) {
        int ind = 0;
        CityList* to_check = game->cityList;
        while (to_check != NULL) {
            if (to_check->city != NULL) {
                give_bonus_city_exploited_tiles(game, to_check->city, tab[ind]);
            }
            to_check = to_check->next;
            ind += 1;
        }
    }
}

void update_city_projects(Game* game) {
    if (game != NULL) {
        CityList* to_check = game->cityList;

        while (to_check != NULL) {
            if (to_check->city != NULL) {
                end_project(game, to_check->city);
            }

            to_check = to_check->next;
        }
    }
}

void give_all_bonuses(Game* game) {
    if (game != NULL) {
        //Partie 1 : Donner les bonus d'exploitation
        TileList** all_exploited_tiles = get_all_exploited_tiles(game);
        give_bonus_exploitations(game, all_exploited_tiles);
        int length = get_city_number(game);
        destroy_all_tilelists(all_exploited_tiles, length);

        //Partie 2 : Donner les bonus de batiments
        CityList* to_check = game->cityList;
        City* city;
        while (to_check != NULL) {
            city = to_check->city;
            give_bonus_city_buildings(game, city);
            to_check = to_check->next;
        }

        //Partie 3 : Octroyer les multiplicateurs et réinitialiser les nouvelles ressources
        to_check = game->cityList;
        while (to_check != NULL) {
            city = to_check->city;
            city->food += (int) ((1 + game->tech_tree->bonus_food_percent/100) * get_new_food(city)) / pow(2, game->poverty);

            //Juste un = car on perd la prod non utilisé à la fin du tour
            city->production = (int) ((1 + game->tech_tree->bonus_prod_percent/100) * get_new_prod(city)) / pow(2, game->poverty);

            city->new_ressources->ressource1 = 0;
            city->new_ressources->ressource2 = 0;
            to_check = to_check->next;
        }
        game->gold += (int) ((1 + game->tech_tree->bonus_gold_percent/100) * get_new_gold(game)) / pow(2, game->poverty);
        game->science += (int) ((1 + game->tech_tree->bonus_science_percent/100) * get_new_science(game)) / pow(2, game->poverty);

        game->new_ressources->ressource1 = 0;
        game->new_ressources->ressource2 = 0;

    }

}

int get_total_unit_maintenance(Game* game) {
    if (game == NULL) {
        return 0;
    }

    int total = 0;
    UnitList* current = game->unitList;

    while (current != NULL) {
        if (current->data != NULL) {
            total += current->data->cost_per_turn;
        }

        current = current->next;
    }

    return total;
}
