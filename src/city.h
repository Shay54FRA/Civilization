#include "unit.h"
#include "building.h"

#ifndef CITY
#define CITY

typedef struct _Project {
    char* type;
    int x;
    int y;
    int production_cost;
} Project;

typedef struct _City {
    int population;
    int pv;
    int food;
    int production;
    Project* project;
} City;

#endif