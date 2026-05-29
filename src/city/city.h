#include "../map/map.h"
#include "../game/game.h"
#include <stdbool.h>
#include <math.h>

#ifndef CITY
#define CITY

#define EXPLOITATION_RANGE ((city->population > 25) + (city->population > 9) + 1)
#define FOOD_NEEDS (2*city->population)
#define CROISSANCE_NEED ((int) ceil((20 + 10*city->population)/pow(1.5, city->basements_number)))
#define CITY_STRENGTH (20 + 10*(city->walls_number > 0))
#define MAX_HP ((int) (10*city->population*pow(2,city->walls_number)))
#define CITY_FOG_RANGE 3 + 2*city->has_phare

typedef struct _BuildList BuildList;

//=============|STRUCTURES|=============//

typedef struct _Project {
    char type; //Minuscule pour unité, Majuscule pour batiment
    Position pos; //Position si jamais le projet est une unité
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
    Position pos;
    TupleRessources* new_ressources;
    bool can_produce_unit;
    bool has_phare;
    bool has_taken_damage;
    Project* project;
    BuildList* buildings;
} City;

typedef struct _CityList { //Liste chaînée des villes du joueur
    City* city;
    struct _CityList* next;
} CityList;


//================|CITY|================//


//########## INIT ##########//

//Création de ville sur une case, un grenier
City* create_city(Position pos); 

//Liberer tous les batiments, le projet et la ville
void destroy_city(City* city);

//######## GETTERS ########//

int get_population(City* city); //Nombre d'habitants
int get_city_pv(City* city); //PV
int get_food(City* city); //Stockage de nourriture de la ville
int get_production(City* city); //Qté de points de productions pour le projet
Project* get_project(City* city); //Récupère le projet en cours de la ville
BuildList* get_buildings_list(City* city); //Liste chaînée des bâtiments de la ville

//########## UTILS ##########//

int get_new_food(City* city);
int get_new_prod(City* city);
bool build_type_exists(char type);
int get_total_pop(CityList* city_list);

//Renvoie la position du min de la distance avec les différents batiments de la ville
int get_distance_to_city(City* city, Position pos);

//Renvoie la ville sur la tuile en question
City* get_city_on_tile(CityList* liste_city, Tile* tile);

bool end_project(Game* game, City* city);
//Si PV <= 0 ou population <= 0 : la sortir de la CityList et la destroy

bool croissance_check(City* city);

//Par tour une ville récupère la moitié de ses PV si elle n'a pas subi de dégâts pendant ce tour
void heal_city(City* city);

void kill_city(Game* game, City* city);

//===============|PROJECT|===============//


//########## INIT ##########//

Project* create_project(char type, Position pos);
void destroy_project(City* city);

//######### GETTERS #########//

char get_project_type(City* city); //Récupère le char d'identification
char* get_project_name(City* city);
int get_production_left(City* city); //Coût de prod restant du projet de la ville

//########## UTILS ##########//

bool start_project(City* city, char type, Position pos); //Renvoie si un projet a été créé ou non (si un projet était déjà actif)

//==============|CITYLIST|==============//

//########## INIT ##########//

void create_city_list(Game* game, City* city); // Création de l'élément
void destroy_city_list(CityList*); // Destruction de l'élément, de la ville, et de son successeur

//######### GETTERS #########//

City* get_city(CityList* lst);
CityList* get_next_city(CityList* lst);

//########## UTILS ##########//

void append_city_list(CityList* to_append, CityList* to_add);

void heal_cities(CityList* city_list);

void update_food(CityList* city_list);

bool check_famine(Game* game, City* city);

void update_croissance(Game* game);

#endif
