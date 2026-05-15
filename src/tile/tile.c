#include <stdio.h>
#include <stdlib.h>

#include "tile.h"
#include "../map/map.h"
#include "../unit/unit.h"
#include "../cli/cli.h"

Tile* create_tile(Position pos, char biome) {
    Tile* new_tile = malloc(sizeof(Tile));
    new_tile->pos = pos;
    new_tile->exploited = false;
    new_tile->city_on = false;
    new_tile->unit = NULL;
    new_tile->biome = biome;
    return new_tile;
}

void destroy_tile(Tile* tile) {
    if (tile != NULL) { //Detruire l'unité dessus
        if (tile->unit != NULL) {
            destroy_unit(tile->unit);
        }
        free(tile);
    }
}

Tile* get_tile(Map* map, Position pos) {
    return map->map[pos.y][pos.x];
}

void print_tile(Tile* tile) {
    if (tile != NULL) {
        print_pos(tile->pos);
        printf(", Unité dessus : %d, Exploité : %d, Biome : %c\n", tile->unit != NULL, tile->exploited != false, tile->biome);
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
        if (tilelist->data == NULL) {
            tilelist->data = tile;
        }
        else {
            TileList* to_check = tilelist;
            while (to_check->next != NULL) {
                to_check = to_check->next;
            }
            to_check->next = create_tilelist(tile);
        }
    }
}

void merge_and_destroy_tilelists(TileList* kept_tilelist, TileList* tilelist_to_free) {
    if (kept_tilelist == NULL) {
        destroy_tilelist(tilelist_to_free);
        return;
    }
    if (tilelist_to_free == NULL) {
        return;
    }

    TileList* to_check = tilelist_to_free;
    while (to_check != NULL) {
        if (to_check->data != NULL) {
            append_tilelist(kept_tilelist, to_check->data);
        }
        to_check = to_check->next;
    }
    destroy_tilelist(tilelist_to_free);
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
