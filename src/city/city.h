#include "../map/map.h"
#include <stdbool.h>

#ifndef CITY
#define CITY

#define EXPLOITATION_RANGE ((city->population > 25) + (city->population > 9) + 1)

typedef struct _BuildList BuildList;

typedef struct _Project {
    char type; //Minuscule pour unité, Majuscule pour batiment
    Position pos;//Egal à l'emplacement de la caserne si unité / le projet est immobile
    int production_cost;
} Project;

typedef struct _City {
    int population;
    int damage;
    int food;
    int production;
    int strength;
    int basements_number;
    int walls_number;
    bool can_produce_unit;
    Project* project;
    BuildList* buildings;
} City;

typedef struct _CityList { //Liste chaînée des villes du joueur
    City* city;
    struct _CityList* next;
} CityList;

City* create_city(Position pos); //Création de ville sur une case, un grenier
int get_population(City* city); //Nombre d'habitants
int get_city_pv(City* city); //PV
int get_food(City* city); //Stockage de nourriture de la ville
int get_production(City* city); //Qté de points de productions pour le projet
Project* get_project(City* city); //Récupère le projet en cours de la ville
BuildList* get_buildings_list(City* city); //Liste chaînée des bâtiments de la ville

bool start_project(City* city, Position pos, char type); //Renvoie si un projet a été créé ou non (si un projet était déjà actif)
void destroy_project(City* city);
char get_project_type(City* city); //Récupère le char d'identification
char* get_project_name(City* city);
Position get_project_pos(City* city);
int get_production_left(City* city); //Coût de prod restant du projet de la ville

City* get_city(CityList* lst);
CityList* get_next_city(CityList* lst);

#endif
