#ifndef MAP
#define MAP

typedef struct _City City;
typedef struct _Unit Unit;

typedef struct _Position {
    int x;
    int y;
} Position;

typedef struct _Tile {
    struct _Position* pos;
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
int get_distance(Position pos1, Position pos2);

Tile* get_tile(Map* map, int x, int y);

#endif
