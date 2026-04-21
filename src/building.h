#include "city.h"


#ifndef BUILDING
#define BUILDING

typedef struct _Building {
    char type;
    City* city_in;
} Building;

typedef struct _BuildList { 
    Building* data;
    struct _BuildList* next;
} BuildList;

void get_bonus_building(Building* build); 

#endif