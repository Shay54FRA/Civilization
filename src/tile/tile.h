#include "../map/map.h"
#include <stdbool.h>

typedef struct _City City;
typedef struct _Unit Unit;

typedef struct _Tile {
    struct _Position pos;
    char biome; // Plaine, Foret, Montagne, Eau, Desert, Toundra
    bool city_on;
    bool exploited; // Utilisé pour l'exploitation
    Unit* unit;
} Tile;

typedef struct _TileList {
    Tile* data;
    struct _TileList* next;
} TileList;

Tile* create_tile(Position pos, char biome);
void destroy_tile(Tile* tile);
Tile* get_tile(Map* map, Position pos);
void print_tile(Tile* tile);

TileList* create_tilelist(Tile* tile);
void destroy_tilelist(TileList* tilelist);
void append_tilelist(TileList* tilelist, Tile* tile);
void merge_and_destroy_tilelists(TileList* kept_tilelist, TileList* tilelist_to_free);
void print_tilelist(TileList* tilelist);
