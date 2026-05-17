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

    destroy_map(ma_carte);

    return 0;
}
