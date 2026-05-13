#include "../map/map.h"

#ifndef GAME
#define GAME

typedef struct _Unit Unit;
typedef struct _UnitList UnitList;
typedef struct _Map Map;
typedef struct _CityList CityList;
typedef struct _BarbarianList BarbarianList;
typedef struct _CampList CampList;
typedef struct _Barbarian Barbarian;
typedef struct _City City;
typedef struct _Building Building;
typedef struct _Tile Tile;
typedef struct _TileList TileList;
typedef struct _Configuration Configuration;
typedef struct _TechTree TechTree;
typedef struct _Camp Camp;

//=============|STRUCTURES|=============//

typedef struct _TupleRessources { //Structure pour connaître toutes les nouvelles ressources et pouvoir appliquer les bonus de l'arbre par dessus
    int ressource1;
    int ressource2;
} TupleRessources;

typedef struct _Game {
    int gold; //Ressource globale partagé entre toutes les villes
    int science; //idem, pour l'arbre de technologie
    int active_turn; //Tour en cours
    TupleRessources* new_ressources;
    Position starting_point; //Utile pour placer les camps
    Map* map; //Carte de la partie
    UnitList* unitList; //Liste des unités du joueur
    CityList* cityList; // Liste des villes du joueur
    Configuration* configuration; // Configuration de la partie (nbr de camps barabares, taille map,...)
    BarbarianList* barbarianList;
    CampList* campList;
    TechTree* tech_tree;
    int active_research_id;   // Le post-it pour retenir le projet en cours
} Game;

//================|TUPLE|================//

//########## INIT ##########//

TupleRessources* create_tuple_ressources(void);
void destroy_tuple_ressources(TupleRessources* structure);

//######### GETTERS #########//

int get_ressource1(TupleRessources* tuple);
int get_ressource2(TupleRessources* tuple);

//================|GAME|================//

//########## INIT ##########//

Game* create_game(Configuration* config);
void destroy_game(Game* game);

//######### GETTERS #########//

int get_turn(Game* game);
Map* get_map(Game* game);
UnitList* get_unitlist(Game* game);
CityList* get_citylist(Game* game);
BarbarianList* get_barblist(Game* game);
CampList* get_camplist(Game* game);

//########## UTILS ##########//

int get_city_number(Game* game);
int get_new_gold(Game* game);
int get_new_science(Game* game);
char* get_name(char type); //donner le nom du batiment ou de l'unité portant ce type
int get_cost(char type); //donner le coût en production de l'unité ou du batiment portant ce type 
int get_entretien_cost(char type); //coût d'entretien en or du batiment ou de l'unité

//######## GAMEPLAY ########//

//Incrémenter tour, attribuer les bonus
void start_turn(Game* game);

//Fin du tour: déplacer les barbares, faire les combats si besoins, générer des barbares, soigner les villes
void end_turn(Game* game);

//########## BONUS ##########//

//Donner le bonus lié au batiment
void give_bonus_building(Game* game, City* city, Building* building);

//Donner le bonus du biome de la tuile
void give_bonus_tile(Game* game, City* city, Tile* tile); 

//Donner tous les bonus de batiments de la ville
void give_bonus_city_buildings(Game* game, City* city);

//Donner tous les bonus d'exploitation de la ville
void give_bonus_city_exploited_tiles(Game* game, City* city, TileList* exploitList);

//Renvoie le tableau de Tile* correspondant à la range
TileList* get_exploitation_range(Game* game, City* city, int range);

//Renvoie le tableau de TileList* pour avoir la liste des tuiles exploités par chacune des villes
TileList** get_all_exploited_tiles(Game* game);

//Met à jour les projets de production des villes
void update_city_projects(Game* game);
int get_total_unit_maintenance(Game* game);

//Faire les bonus de toutes les villes : utilise toutes les fonctions au dessus
void give_all_bonuses(Game* game); 

#endif
