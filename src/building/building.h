#include "../map/map.h" //Le typedef ne suffit pas car on appelle Position et non Position*
#include <stdbool.h>

#ifndef BUILDING
#define BUILDING

/*                              BONUS                          Entretien (or) Coût (prod) Techno requise
G : Grenier          = +3 food/tour et seuil croissance /1.5 ||      1      ||    30    ||     -
A : Atelier          = +3 prod/tour                          ||      1      ||    40    ||     -
B : Bibliothèque     = +4 science/tour                       ||      1      ||    50    || Ecriture 
M : Marché           = +3 gold/tour                          ||      1      ||    40    || Commerce 
C : Caserne          = Produire des unités                   ||      2      ||    60    ||     -
R : Rempart/Muraille = Double PV ville                       ||      2      ||    80    || Maçonnerie
*/

typedef struct _Building {
    char type;
} Building;

typedef struct _BuildList { 
    Building* data;
    struct _BuildList* next;
} BuildList;

Building* create_building(char type);
void destroy_building(Building* build);
char get_building_type(Building* build);
void print_building(Building* build); 

BuildList* create_buildlist(Building* build, BuildList* next);
void destroy_buildlist(BuildList* lst);
void append_buildlist(BuildList* lst, Building* build);
Building* get_buildlist_data(BuildList* lst);
BuildList* get_buildlist_next(BuildList* lst);
bool buildlist_contains(BuildList* lst, char type);
void print_buildlist(BuildList* lst);

#endif
