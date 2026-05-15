#include "game.h"
#include "../building/building.h"
#include "../map/map.h"
#include "../tile/tile.h"
#include "../city/city.h"
#include "../unit/unit.h"
#include "../technology/technology.h"
#include "../configuration/configuration.h"
#include <stdlib.h>
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
            Position pos = {1,1};
            game->science = 0;
            game->gold = 0;
            game->active_research_id = -1;
            game->active_turn = 1;
            game->configuration = config;
            game->map = create_map(get_width(config), get_height(config), get_seed(config));
            game->barbarianList = NULL; //Pas encore créé
            game->campList = NULL; //Pas encore créé
            game->starting_point = pos; //A modifier
            game->cityList = NULL; 
            game->unitList = NULL; //Pas encore créé
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
        // Rajouter la destruction des units, des camps de barbares et des barbares
        // A rajouter destroy_citylist()
        destroy_map(game->map);
        destroy_configuration(game->configuration);
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

int get_city_number(Game* game) {
    if (game != NULL) {
        int rep = 0;
        CityList* to_check = game->cityList;
        while (to_check != NULL) {
            rep += 1;
            to_check = to_check->next;
        }
        return rep;
    }
    return -1;
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


void* get_nearest_target(Game* game, Barbarian* barb); //void* pour renvoyer au choix Unit ou City
/* Pour les villes on prendra le min de la distance avec chacun des batiments de la ville */
void move_barbarian(Game* game, Barbarian* barb, void* target); //Calculer la direction nécéssaire pour se rapprocher et l'appliquer

TileList* get_exploitation_range(Game* game, City* city, int range) {
    if (city != NULL) {
        if (city->buildings != NULL) {
            TileList* rep = create_tilelist(NULL);
            if (rep != NULL) {
                BuildList* to_check = city->buildings;
                Building* build;
                Position pos;
                Tile* tile;
                TileList* exploit_of_building;
                while(to_check != NULL) {
                    build = to_check->data;
                    pos = build->pos;
                    tile = get_tile(game->map, pos);
                    exploit_of_building = get_exploited_tiles(game->map, tile, range);
                    merge_and_destroy_tilelists(rep, exploit_of_building);
                    to_check = to_check->next;
                }
            }
            return rep;
        }
    }
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

            city->food += city->new_ressources->ressource1
                        * (100 + game->tech_tree->bonus_food_percent) / 100;

            //Juste un = car on perd la prod non utilisé à la fin du tour
            city->production = city->new_ressources->ressource2
                             * (100 + game->tech_tree->bonus_prod_percent) / 100;

            city->new_ressources->ressource1 = 0;
            city->new_ressources->ressource2 = 0;
            to_check = to_check->next;
        }

        game->gold += game->new_ressources->ressource1
                    * (100 + game->tech_tree->bonus_gold_percent) / 100;

        game->science += game->new_ressources->ressource2
                       * (100 + game->tech_tree->bonus_science_percent) / 100;

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

