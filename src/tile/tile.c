#include "tile.h"
#include "../map/map.h"
#include <stdio.h>
#include <stdlib.h>

Tile* create_tile(Position pos, char biome) {
    Tile* new_tile = malloc(sizeof(Tile));
    new_tile->pos = pos;
    new_tile->city_on = NULL;
    new_tile->unit = NULL;
    new_tile->biome = biome;
    return new_tile;
}

void destroy_tile(Tile* tile) {
    free(tile);
}

Tile* get_tile(Map* map, Position pos) {
    return map->map[pos.y][pos.x];
}

void print_tile(Tile* tile) {
    if (tile != NULL) {
        print_pos(tile->pos);
        printf(", Unité dessus : %d, Exploité : %d, Biome : %c\n", tile->unit != NULL, tile->city_on != NULL, tile->biome);
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

TileList* get_neighbors(Map* map, Tile* tuile, bool for_exploitation) {
    Position pos = tuile->pos;
    if (map != NULL) {
        TileList* rep = create_tilelist(tuile);
        for (int x = pos.x-1; x<pos.x+2; x++) {
            for (int y = pos.y-1; y<pos.y+2; y++) {
                if (x >= 0 && x < map->length && y >= 0 && y < map->height && (x != pos.x || y != pos.y)) {
                    Tile* new_tile = map->map[y][x];
                    if (x == pos.x || y == pos.y){ //On règle les positions basiques autour
                        if (for_exploitation && !(new_tile->exploited)) {
                            new_tile->
                            append_tilelist(rep, new_tile);
                        }
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