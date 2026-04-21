#ifndef MAP
#define MAP

#include "unit.h"
#include "city.h"

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

Map* create_map(void); //Paramètres à revoir
void destroy_map(Map* map);

Tile* get_tile(Map* map, int x, int y);

#endif
