#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

int main(void){
    assert(strcmp(get_name('C'), "Caserne") == 0);
    assert(get_cost('C') == 60);
    assert(get_entretien_cost('C') == 2);
    assert(get_entretien_cost('G') == 1);
    assert(get_entretien_cost('Z') == -1);
    return 0;
}
