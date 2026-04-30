#include "map.h"
#include <stdlib.h>
#include <stdio.h>

Tile* create_tile(Position pos, char biome) {
    Tile* new_tile = malloc(sizeof(Tile));
    new_tile->pos = pos;
    new_tile->city_on = NULL;
    new_tile->unit = NULL;
    new_tile->biome = biome;
    return new_tile;
}

Tile* get_tile(Map* map, Position pos) {
    return map->map[pos.y][pos.x];
}

Map* create_map(int width, int height, int seed){

    srand(seed); //Decide de la génération aléatoire selon la seed donnée. (Permet de rejouer la partie)

    Map* m = malloc(sizeof(Map));
    m->height=height;
    m->length=width;
    m->map=malloc(sizeof(Tile**)*height);

    for(int i=0;i<height;i=i+1){
        m->map[i]=malloc(sizeof(Tile*)*width);

        for(int j=0;j<width;j=j+1){
            Position pos = {j, i}; // X = Colonnes (largeur), Y = Lignes (hauteur)
            //Initialisation biome
            int alea = rand() % 100;
            if (alea < 40) {
                m->map[i][j] = create_tile(pos, 'P'); // 40% de chance d'avoir une Plaine
            }
            else if (alea < 50) {
                m->map[i][j] = create_tile(pos, 'F'); // 10% de chance d'avoir une Forêt
            }
            else if (alea < 60) {
                m->map[i][j] = create_tile(pos, 'M'); // 10% de chance d'avoir une Montagne
            }
            else if (alea < 70) {
                m->map[i][j] = create_tile(pos, 'E'); // 10% de chance d'avoir de l'Eau
            }
            else if (alea < 80) {
                m->map[i][j] = create_tile(pos, 'D'); // 10% de chance d'avoir un Désert
            }
            else{
                m->map[i][j] = create_tile(pos, 'T'); // 20% de chance d'avoir une Toundra
            }
        }
    }
    return m;
}


void destroy_map(Map* m) {
    if (m != NULL) {
        for (int i = 0; i < m->height; i++) {
            for(int j=0; j< m->length;j++){
                free(m->map[i][j]);
            }
            free(m->map[i]); 
        }

        free(m->map);
        
        free(m);
    }
}


// Définition des couleurs ANSI
#define COLOR_RESET    "\x1b[0m"
#define COLOR_EAU      "\x1b[38;5;33m"   // Bleu plus clair/océan
#define COLOR_PLAINE   "\x1b[38;5;114m"  // Vert tendre
#define COLOR_FORET    "\x1b[38;5;22m"   // Vert sapin très sombre
#define COLOR_MONTAGNE "\x1b[38;5;244m"  // Gris roche
#define COLOR_DESERT   "\x1b[38;5;220m"  // Jaune sable
#define COLOR_TOUNDRA  "\x1b[38;5;159m"  // Bleu givré glacé

void print_pos(Position pos) {
    printf("Position : (%d, %d)", pos.x, pos.y);
}

void print_tile(Tile* tile) {
    if (tile != NULL) {
        print_pos(tile->pos);
        printf(", Unité dessus : %d, Exploité : %d, Biome : %c\n", tile->unit != NULL, tile->city_on != NULL, tile->biome);
    }
}

void print_map(Map* m) {
    if (m == NULL || m->map == NULL) return;

    for (int i = 0; i < m->height; i++) {
        
        for (int j = 0; j < m->length; j++) {
            
            Tile* current_tile = m->map[i][j];


            switch(current_tile->biome) {
                case 'E': printf("%s ~ %s", COLOR_EAU, COLOR_RESET); break;
                case 'P': printf("%s . %s", COLOR_PLAINE, COLOR_RESET); break;
                case 'F': printf("%s # %s", COLOR_FORET, COLOR_RESET); break;
                case 'M': printf("%s ^ %s", COLOR_MONTAGNE, COLOR_RESET); break;
                case 'D': printf("%s x %s", COLOR_DESERT, COLOR_RESET); break;
                case 'T': printf("%s * %s", COLOR_TOUNDRA, COLOR_RESET); break;
                default:  printf(" ? "); break; // Sécurité si un biome inconnu s'est glissé
            }

        }
        printf("\n");
    }
}

TileList* create_tilelist(Tile* tuile) {
    TileList* tilelist = malloc(sizeof(TileList));
    if (tilelist != NULL) {
        tilelist->data = tuile;
        tilelist->next = NULL;
    }
    return tilelist;
}

void destroy_tilelist(TileList* tilelist) { //Ne pas free les tile !!
    if (tilelist != NULL) {
        destroy_tilelist(tilelist->next);
        free(tilelist);
    }
}

void append_tilelist(TileList* tilelist, Tile* tile) {
    if (tile != NULL && tilelist != NULL) {
        TileList* new_tilelist = create_tilelist(tile);
        TileList* to_check = tilelist;
        while (to_check->next != NULL) {
            to_check = to_check->next;
        }
        to_check->next = new_tilelist;
    }
}

void print_tilelist(TileList* tilelist){
    TileList* to_check = tilelist;
    if (to_check != NULL) {
        printf("Liste de tuiles : \n");
        while (to_check != NULL) {
            print_tile(to_check->data);
            to_check = to_check->next;
        }
    }
}

TileList* get_neighbors(Map* map, Tile* tuile) {
    Position pos = tuile->pos;
    if (map != NULL) {
        TileList* rep = create_tilelist(tuile);
        for (int x = pos.x-1; x<pos.x+2; x++) {
            for (int y = pos.y-1; y<pos.y+2; y++) {
                if (x >= 0 && x < map->length && y >= 0 && y < map->height && (x != pos.x || y != pos.y)) {
                    Tile* new_tile = map->map[y][x];
                    if (x == pos.x || y == pos.y){ //On règle les positions basiques autour
                        append_tilelist(rep, new_tile);
                    }
                    else if (pos.y % 2 == 0 && x == pos.x-1) {
                        append_tilelist(rep, new_tile);
                    }
                    else if (pos.y % 2 == 1 && x == pos.x+1) {
                        append_tilelist(rep, new_tile);
                    }
                }
            }
        }
        return rep;
    }
    return NULL;
}