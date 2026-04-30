#ifndef GAME
#define GAME

typedef struct _Position Position;
typedef struct _UnitList UnitList;
typedef struct _Map Map;
typedef struct _CityList CityList;
typedef struct _BarbarianList BarbarianList;
typedef struct _CampList CampList;
typedef struct _Barbarian Barbarian;
typedef struct _City City;
typedef struct _Building Building;
typedef struct _TileList TileList;

typedef struct _Turn { //Structure pour connaître toutes les nouvelles ressources et pouvoir appliquer les bonus de l'arbre par dessus
    int gold;
    int science;
    int* new_foods; //Tableau pour l'attribuer à chacun des villes
    int* new_prods; //idem
} Turn;

typedef struct _Game {
    int gold; //Ressource globale partagé entre toutes les villes
    int science; //idem, pour l'arbre de technologie
    int active_turn; //Tour en cours
    int max_turn; //Tour maximum, défaite si dépassé
    Position* starting_point; //Utile pour placer les camps
    Map* map; //Carte de la partie
    UnitList* unitList; //Liste des unités du joueur
    CityList* cityList; // Liste des villes du joueur
    int barbarian_camps_nbr;
    BarbarianList* barbarianList;
    CampList* campList;
} Game;

Game* create_game(void);
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

void give_bonus_building(Game* game, City* city, Building* building); //Donner le bonus lié au batiment
TileList* get_exploitation_range(Game* game, City* city, int range); //Renvoie le tableau de Tile* correspondant à la range
void give_bonus_city(City* city); //Donner le bonus de tous les batiments de la ville + terres exploités
void give_all_bonuses(Game* game); //Faire les bonus de toutes les villes



#endif
