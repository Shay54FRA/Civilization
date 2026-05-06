#include "game.h"
#include "../building/building.h"
#include "../map/map.h"
#include "../tile/tile.h"
#include "../city/city.h"
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

void give_bonus_building(Game* game, City* city, Building* building) {
    switch (building->type) {
        case 'G': city->new_ressources->ressource1 += 3; break;
        case 'A': city->new_ressources->ressource2 += 3; break;
        case 'B': game->new_ressources->ressource2 += 4; break;
        case 'M': game->new_ressources->ressource1 += 3; break;
        case 'C': city->can_produce_unit = true; break;
        case 'R': city->walls_number += 1; break;
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

void give_bonus_city(City* city); //Donner le bonus de tous les batiments de la ville + terres exploités
void give_all_bonuses(Game* game); //Faire les bonus de toutes les villes
