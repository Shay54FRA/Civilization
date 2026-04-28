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
    char biome; // Plaine, Foret, Montagne, Eau, Desert, Toundra
    City* city_on;
    Unit* unit;
} Tile;

typedef struct _Map {
    Tile*** map; // Triple pointeur : un tableau de tableaux de pointeurs
    int height;
    int length;
} Map;

Map* create_map(int width, int height, int seed);
void destroy_map(Map* map);
void print_map(Map* map);

int get_distance(Position pos1, Position pos2);
Tile* get_tile(Map* map, int x, int y);

#endif
