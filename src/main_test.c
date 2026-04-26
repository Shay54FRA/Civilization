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
    /*------TEST BUILDING------*/
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

    /*--------TEST CITY--------*/

    Position pos2 = {0,1};
    City* city1 = create_city(pos);
    assert(get_production(city1) == 0);
    assert(get_population(city1) == 1);
    assert(get_food(city1) == 0);
    assert(get_project(city1) == NULL);
    assert(get_city_pv(city1) == 10);
    assert(start_project(city1, pos, 'G'));
    print_buildlist(city1->buildings);

    /*--------TEST GAME--------*/

    assert(strcmp(get_name('C'), "Caserne") == 0);
    assert(get_cost('C') == 60);
    assert(get_entretien_cost('C') == 2);
    assert(get_entretien_cost('G') == 1);
    assert(get_entretien_cost('Z') == -1);

    /*--------TEST UNIT--------*/
    /*--------TEST MAP---------*/
    /*-----TEST BARBARIAN------*/
    return 0;
}
