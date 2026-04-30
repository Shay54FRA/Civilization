#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    Map* ma_carte = create_map(30, 15, 42); //Width = 30, Height = 15, Seed = 42
    
    print_map(ma_carte);

    Position pos = {1,1};

    Tile* ma_tuile = get_tile(ma_carte, pos);
    print_tile(ma_tuile);
    TileList* tuiles_voisines = get_neighbors(ma_carte, ma_tuile);
    print_tilelist(tuiles_voisines);

    destroy_tilelist(tuiles_voisines);
    destroy_map(ma_carte);
    return 0;
}
