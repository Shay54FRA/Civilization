#include <stdbool.h>


#ifndef MAP
#define MAP

#include <ncurses.h>

/*
La map apparaîtra sur l'interface graphique avec des hexagones,
Mais comment représenter ça en machine hmmmmm...
Il faut voir ça comme des carrés décalés
    _____________________________
   |__|__|__|__|__|__|__|__|__|__|  <- y = 0
    |__|__|__|__|__|__|__|__|__|__|
   |__|__|__|()|__|__|__|__|__|__|   
    |__|__|__|__|__|__|__|__|__|__|
   |__|__|__|__|__|__|__|__|__|__|
    |__|()|__|__|__|__|__|__|__|__|
   |__|__|__|__|__|__|__|__|__|__|   
    ^x = 0
Case (x,y) a accès à (s'ils existent): (x+1,y), (x-1,y), (x,y+1), (x,y-1), (x+(-1)^(y+1),y+1), (x+(-1)^(y+1),y-1)
On supposera toujours qu'on construit la carte en partant d'en haut à gauche
Ex : (3,4) a accès à {(2,4), (4,4), (3,3), (3,5), (2,5), (2,3)} 
*/

typedef struct _City City;
typedef struct _Unit Unit;
typedef struct _Tile Tile;
typedef struct _TileList TileList;

typedef struct _Position {
    int x;
    int y;
} Position;

typedef struct _Map {
    Tile*** map; // Triple pointeur : un tableau de tableaux de pointeurs
    int height;
    int length;
} Map;

Map* create_map(int width, int height, int seed, int nb_camps);
void print_map_cli(WINDOW* win,Map* m, Position cursor);
void smooth_map(Map* m);

/*
Explication de la fonction smooth.




*/
void destroy_map(Map* map);
//void print_pos(Position pos);
void print_tile(WINDOW* win,Tile* tile);

Position* create_position(int x, int y);
void destroy_position(Position* pos);


Position get_starting_city_pos(Map* map); // Renvoie la position de la ville de départ


// // Définition des couleurs ANSI
// #define COLOR_RESET   "\x1b[0m"
// #define BG_EAU        "\x1b[48;5;33m\x1b[30m"   // Fond Bleu
// #define BG_PLAINE     "\x1b[48;5;114m\x1b[30m"  // Fond Vert clair
// #define BG_FORET      "\x1b[48;5;22m\x1b[37m"   // Fond Vert foncé
// #define BG_MONTAGNE   "\x1b[48;5;244m\x1b[30m"  // Fond Gris
// #define BG_DESERT     "\x1b[48;5;220m\x1b[30m"  // Fond Jaune
// #define BG_TOUNDRA    "\x1b[48;5;159m\x1b[30m"  // Fond bleu givré

// #define COLOR_VILLE   "\x1b[45;1;37m" // Fond Magenta

// Définition des identifiants de couleurs ncurses
#define COLOR_EAU 1
#define COLOR_PLAINE 2
#define COLOR_FORET 3
#define COLOR_MONTAGNE 4
#define COLOR_DESERT 5
#define COLOR_TOUNDRA 6
#define COLOR_VILLE 7
#define COLOR_CURSEUR 8
#define COLOR_BROUILLARD 9
#define COLOR_ROUGE 10
#define COLOR_JAUNE 11
#define COLOR_VERT 12

int get_distance(Position pos1, Position pos2);
void reset_exploitation(Map* map); // Remet toute les tuiles à false pour l'exploitation
TileList* get_tiles_at_range(Map* map, Position pos, int range);
TileList* get_exploited_tiles(Map* map, Tile* tuile, int range); 
void mark_exploited_tiles(TileList* tile_list);
void mark_seen_tiles(Map* map, Position pos, int range);
Position dijkstra(Map* map, Position pos_start, Position pos_end);



#endif
