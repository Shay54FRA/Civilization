#include "tile.h"

int main(void){
    Position pos1 = {0,0};
    Position pos2 = {1,0};
    Position pos3 = {0,1};
    Position pos4 = {1,1};
    Tile* new_tile1 = create_tile(pos1, 'P');
    Tile* new_tile2 = create_tile(pos2, 'P');
    Tile* new_tile3 = create_tile(pos3, 'P');
    Tile* new_tile4 = create_tile(pos4, 'P');

    print_tile(new_tile1);
    
    TileList* new_tilelist1 = create_tilelist(new_tile1);
    TileList* new_tilelist2 = create_tilelist(new_tile2);
    append_tilelist(new_tilelist1, new_tile3);
    append_tilelist(new_tilelist2, new_tile4);

    print_tilelist(new_tilelist1);
    print_tilelist(new_tilelist2);

    merge_and_destroy_tilelists(new_tilelist1, new_tilelist2);
    print_tilelist(new_tilelist1);

    destroy_tilelist(new_tilelist1);
    destroy_tile(new_tile1);
    destroy_tile(new_tile2);
    destroy_tile(new_tile3);
    destroy_tile(new_tile4);
}
