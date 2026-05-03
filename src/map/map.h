#ifndef MAP
#define MAP

/*
La map apparaîtra sur l'interface graphique avec des hexagones,
Mais comment représenter ça en machine hmmmmm...
Il faut voir ça comme des carrés décalés
    _____________________________
   |__|__|__|__|__|__|__|__|__|__|
    |__|__|__|__|__|__|__|__|__|__|
   |__|__|__|()|__|__|__|__|__|__|   
    |__|__|__|__|__|__|__|__|__|__|
   |__|__|__|__|__|__|__|__|__|__|
    |__|()|__|__|__|__|__|__|__|__|
   |__|__|__|__|__|__|__|__|__|__|   <- y = 0
    ^x = 0
Case (x,y) a accès à (s'ils existent): (x+1,y), (x-1,y), (x,y+1), (x,y-1), (x+(-1)^(y+1),y+1), (x+(-1)^(y+1),y-1)
On supposera toujours qu'on construit la carte en partant d'en pas à droite avec une ligne non décalée
Ex : (3,4) a accès à {(2,4), (4,4), (3,3), (3,5), (2,5), (2,3)} 
*/

typedef struct _City City;
typedef struct _Unit Unit;

typedef struct _Position {
    int x;
    int y;
} Position;

typedef struct _Tile {
    struct _Position pos;
    char biome; // Plaine, Foret, Montagne, Eau, Desert, Toundra
    City* city_on; // Utilisé pour l'exploitation
    Unit* unit;
} Tile;

typedef struct _TileList {
    Tile* data;
    struct _TileList* next;
} TileList;

typedef struct _Map {
    Tile*** map; // Triple pointeur : un tableau de tableaux de pointeurs
    int height;
    int length;
} Map;

Tile* create_tile(Position pos, char biome);
Map* create_map(int width, int height, int seed);
void destroy_map(Map* map);
void print_pos(Position pos);
void print_tile(Tile* tile);

// Définition des couleurs ANSI
#define COLOR_RESET   "\x1b[0m"
#define BG_EAU        "\x1b[48;5;33m\x1b[30m"   // Fond Bleu
#define BG_PLAINE     "\x1b[48;5;114m\x1b[30m"  // Fond Vert clair
#define BG_FORET      "\x1b[48;5;22m\x1b[37m"   // Fond Vert foncé
#define BG_MONTAGNE   "\x1b[48;5;244m\x1b[30m"  // Fond Gris
#define BG_DESERT     "\x1b[48;5;220m\x1b[30m"  // Fond Jaune
#define BG_TOUNDRA    "\x1b[48;5;159m\x1b[30m"  // Fond bleu givré

#define COLOR_VILLE   "\x1b[45;1;37m" // Fond Magenta
#define COLOR_UNITE   "\x1b[41;1;37m" // Fond Rouge

void print_map(Map* m, Position cursor);

int get_distance(Position pos1, Position pos2);
Tile* get_tile(Map* map, Position pos);

TileList* create_tilelist(Tile* tile);
void destroy_tilelist(TileList* tilelist);
void append_tilelist(TileList* tilelist, Tile* tile);
void print_tilelist(TileList* tilelist);
TileList* get_neighbors(Map* map, Tile* tuile);

#endif
