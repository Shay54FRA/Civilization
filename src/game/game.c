#include "game.h"
#include "../building/building.h"
#include "../map/map.h"
#include "../tile/tile.h"
#include "../city/city.h"
#include "../technology/technology.h"
#include "../configuration/configuration.h"
#include <stdlib.h>
#include <stdio.h>


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



void* get_nearest_target(Game* game, Barbarian* barb); //void* pour renvoyer au choix Unit ou City
/* Pour les villes on prendra le min de la distance avec chacun des batiments de la ville */
void move_barbarian(Game* game, Barbarian* barb, void* target); //Calculer la direction nécéssaire pour se rapprocher et l'appliquer

TileList* get_exploitation_range(Game* game, City* city, int range) {
    if (city != NULL) {
        if (city->buildings != NULL) {
            TileList* rep = malloc(sizeof(TileList));
            if (rep != NULL) {
                BuildList* to_check = city->buildings;
                while(to_check != NULL) {
                    Building* build = to_check->data;
                    Position pos = build->pos;
                    Tile* tuile = get_tile(game->map, pos);
                    TileList* exploit_of_building = get_exploited_tiles(game->map, tuile, range);
                    merge_and_destroy_tilelists(rep, exploit_of_building);
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
        for (int range = 0; range < 4; range++) {
            CityList* to_check = game->cityList;
            for (int ind_city = 0; ind_city < len_city; ind_city++) {
                City* city = to_check->city;
                if (range <= EXPLOITATION_RANGE) {
                    merge_and_destroy_tilelists(listeTilelist[ind_city], get_exploitation_range(game, city, range));
                }
                to_check = to_check->next;
            }
        }
        return listeTilelist;
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

void give_all_bonuses(Game* game) {
    if (game != NULL) {
        //Partie 1 : Donner les bonus d'exploitation
        TileList** all_exploited_tiles = get_all_exploited_tiles(game);
        give_bonus_exploitations(game, all_exploited_tiles);

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
            city->food += (int) (1 + game->tech_tree->bonus_food_percent/100) * city->new_ressources->ressource1;

            //Juste un = car on perd la prod non utilisé à la fin du tour
            city->production = (int) (1 + game->tech_tree->bonus_prod_percent/100) * city->new_ressources->ressource2;

            city->new_ressources->ressource1 = 0;
            city->new_ressources->ressource2 = 0;
            to_check = to_check->next;
        }
        game->gold += (int) (1 + game->tech_tree->bonus_gold_percent/100) * game->new_ressources->ressource1;
        game->science += (int) (1 + game->tech_tree->bonus_science_percent/100) * city->new_ressources->ressource2;

        game->new_ressources->ressource1 = 0;
        game->new_ressources->ressource2 = 0;


    }

}
