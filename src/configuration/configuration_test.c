#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "configuration.h"

int main(void){
    Configuration *config = create_configuration(
        50,   // largeur
        30,   // hauteur
        1234, // graine
        200,  // nbr tours
        3,    // nbr_camps_barbares
        1     // mode SDL
    );

    // Test si la config a bien été créée.
    assert(config != NULL);

     // Tests des getters
    assert(get_width(config) == 50);
    assert(get_height(config) == 30);
    assert(get_seed(config) == 1234);
    assert(get_nbr_tours(config) == 200);
    assert(get_nbr_camps_barbares(config) == 3);
    assert(get_mode(config) == 1);

    // Libération mémoire
    destroy_configuration(config);

    return 0;
}

