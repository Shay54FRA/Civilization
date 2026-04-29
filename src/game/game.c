#include "game.h"
#include "../building/building.h"
#include "../city/city.h"
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
        case 'G': city->food += 3; break;
        case 'A': city->production += 3; break;
        case 'B': game->science += 4; break;
        case 'M': game->gold += 3; break;
        case 'C': city->can_produce_unit = true; break;
        case 'R': city->walls_number += 1; break;
    }
}