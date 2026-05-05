#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "configuration.h"



int main(void){
    assert(get_height() == 0);
    assert(get_width() == 0);
    assert(get_seed() >= 0); // verif que la seed est bien un nbr entier positif
    assert(get_nbr_tours() == 1);
    assert(get_nbr_camps_barbares() == -1);
    assert(get_mode() == 1);
    return 0;
}
