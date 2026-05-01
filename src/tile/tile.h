#include "../map/map.h"
#include <stdbool.h>

typedef struct _City City;
typedef struct _Unit Unit;

typedef struct _Tile {
    struct _Position pos;
    char biome; // Plaine, Foret, Montagne, Eau, Desert, Toundra
    bool exploited; // Utilisé pour l'exploitation
    Unit* unit;
} Tile;

typedef struct _TileList {
    Tile* data;
    struct _TileList* next;
} TileList;

Tile* create_tile(Position pos, char biome);
Tile* get_tile(Map* map, Position pos);
void print_tile(Tile* tile);

TileList* create_tilelist(Tile* tile);
void destroy_tilelist(TileList* tilelist);
void append_tilelist(TileList* tilelist, Tile* tile);
void print_tilelist(TileList* tilelist);
TileList* get_neighbors(Map* map, Tile* tuile, bool for_exploitation); //Booléen en plus pour déterminer s'il faut marquer les tuiles en plus
