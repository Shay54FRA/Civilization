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

typedef struct _TupleRessources { //Structure pour connaître toutes les nouvelles ressources et pouvoir appliquer les bonus de l'arbre par dessus
    int ressource1;
    int ressource2;
} TupleRessources;

typedef struct _Game {
    int gold; //Ressource globale partagé entre toutes les villes
    int science; //idem, pour l'arbre de technologie
    int active_turn; //Tour en cours
    TupleRessources* new_ressources;
    int max_turn; //Tour maximum, défaite si dépassé
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

TupleRessources* create_tuple_ressources(void);
void destroy_tuple_ressources(TupleRessources* structure);

Game* create_game(Configuration* config);
void destroy_game(Game* game);

int get_turn(Game* game);
Map* get_map(Game* game);
UnitList* get_unitlist(Game* game);
CityList* get_citylist(Game* game);
BarbarianList* get_barblist(Game* game);
CampList* get_camplist(Game* game);

char* get_name(char type); //donner le nom du batiment ou de l'unité portant ce type
int get_cost(char type); //donner le coût en production de l'unité ou du batiment portant ce type 
int get_entretien_cost(char type); //coût d'entretien en or du batiment ou de l'unité
void* get_nearest_target(Game* game, Barbarian* barb); //void* pour renvoyer au choix Unit ou City
/* Pour les villes on prendra le min de la distance avec chacun des batiments de la ville */
void move_barbarian(Game* game, Barbarian* barb, void* target); //Calculer la direction nécéssaire pour se rapprocher et l'appliquer
void colonize(Game* game, Unit* colon); //Vérifier la condition de distance, créer la ville et l'ajouter à game

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

//Faire les bonus de toutes les villes : utilise toutes les fonctions au dessus
void give_all_bonuses(Game* game); 

#endif
