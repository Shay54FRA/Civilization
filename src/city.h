#ifndef CITY
#define CITY

typedef struct _Position Position;
typedef struct _BuildList BuildList;
typedef struct _Position Position;

typedef struct _Project {
    char type; //Minuscule pour unité, Majuscule pour batiment
    Position* pos;//Egal à l'emplacement de la caserne si unité
    int production_cost;
} Project;

typedef struct _City {
    int population;
    int pv;
    int food;
    int production;
    Project* project;
    BuildList* buildings;
} City;

typedef struct _CityList { //Liste chaînée des villes du joueur
    City* city;
    struct _CityList* next;
} CityList;

City* create_city(void); //Création de ville sur une case, un grenier
int get_population(City* city); //Nombre d'habitants
int get_city_pv(City* city); //PV
int get_food(City* city); //Stockage de nourriture de la ville
int get_production(City* city); //Qté de points de productions pour le projet
Project* get_project(City* city); //Récupère le projet en cours de la ville
BuildList* get_buildings_list(City* city); //Liste chaînée des bâtiments de la ville

char* get_project_type(Project* project); //Récupère le char d'identification
int get_project_pos(Project* project);
int get_production_cost(Project* project); //Coût de prod restant du projet

City* get_city(CityList* lst);
City* get_next_city(CityList* lst);

#endif
