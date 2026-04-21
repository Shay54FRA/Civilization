#ifndef GAME
#define GAME

#include "unit.h"
#include "map.h"
#include "city.h"
#include "building.h"

typedef struct _Game {
    int gold;
    int science;
    UnitList* unitList; //Liste des unités du joueur
    CityList* cityList; // Liste des villes du joueur
} Game;

Game* create_game(void);
void destroy_game(Game* game);

void give_bonus_building(City* city, Building* building); //Donner le bonus lié au batiment
void give_bonus_city(City* city); //Donner le bonus de tous les batiments de la ville + terres exploités
void give_all_bonuses(Game* game); //Faire les bonus de toutes les villes


#endif
