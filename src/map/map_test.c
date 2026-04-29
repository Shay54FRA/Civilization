#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    Map* ma_carte = create_map(30, 15, 42); //Width = 30, Height = 15, Seed = 42
    
    print_map(ma_carte);
    
    destroy_map(ma_carte);
    
    return 0;
}
