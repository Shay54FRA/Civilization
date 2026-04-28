#include "building.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


int main(void) {
    Position pos = {0,0};   
    Building* build1 = create_building('C', pos);
    assert(build1 != NULL);
    assert(get_building_type(build1) == 'C');
    assert(get_building_pos(build1).x == 0);
    assert(get_building_pos(build1).y == 0);
    printf("Etat de build1 -> ");
    print_building(build1);

    Building* build2 = create_building('G', pos);

    BuildList* liste = create_buildlist(build2);
    assert(liste != NULL);
    assert(get_buildlist_data(liste) == build2);
    assert(get_buildlist_next(liste) == NULL);
    printf("Etat de la liste -> ");
    print_buildlist(liste);
    append_buildlist(liste, build1);
    printf("Etat de la liste après ajout -> ");
    print_buildlist(liste);

    destroy_buildlist(liste);
}
