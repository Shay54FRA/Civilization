#include "unit.h"
#include "city.h"

#ifndef MAP
#define MAP

typedef struct _Tile {
    int x;
    int y;
    char biome;
    City* city_on;
    Unit* unit;
} Tile;

typedef struct _Map {
    Tile** map;
    int height;
    int length;
} Map;


#endif