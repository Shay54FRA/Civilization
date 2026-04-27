#include "building.h"
#include "game.h"
#include "city.h"
#include "unit.h"
#include "map.h"
#include "barbarian.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

int main(void) {
    /* TEST BUILDING.H */
    Position pos = {0,0};   
    Building* build1 = create_building('C', pos);
    assert(build1 != NULL);
    assert(get_building_type(build1) == 'C');
    assert(strcmp(get_building_name(build1), "Caserne") == 0);
    assert(get_building_pos(build1).x == 0);
    assert(get_building_pos(build1).y == 0);
    assert(get_building_entretien_cost(build1) == 2);
    printf("Etat de build1 -> ");
    print_building(build1);

    Building* build2 = create_building('G', pos);
    assert(get_building_entretien_cost(build2) == 1);

    Building* build_fake = create_building('Z', pos);
    assert(get_building_entretien_cost(build_fake) == -2);

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
    destroy_building(build_fake);
    return 0;
}
