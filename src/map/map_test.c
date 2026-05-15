#include "map.h"
#include "../tile/tile.h"
#include "../cli/cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    
    Map* ma_carte = create_map(10, 10, 42,5);

    Position cursor = {0, 0}; // Position initiale de la caméra

    print_map_cli(ma_carte,cursor);

    // Tile* ma_tuile = get_tile(ma_carte, pos);
    // print_tile(ma_tuile);
    // TileList* tuiles_voisines = get_neighbors(ma_carte, ma_tuile);
    // print_tilelist(tuiles_voisines);

    // destroy_tilelist(tuiles_voisines);

    // Position pos = {1,1};
    // TileList* zone_exploitation = get_exploited_tiles(ma_carte, get_tile(ma_carte,pos), 1);
    // print_tilelist(zone_exploitation);

    // destroy_tilelist(zone_exploitation);
    destroy_map(ma_carte);

    return 0;
}
