#include "city.h"
#include "../building/building.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    Position pos = {0,0};
    City* city1 = create_city(pos);
    assert(get_production(city1) == 0);
    assert(get_population(city1) == 1);
    assert(get_food(city1) == 0);
    assert(get_project(city1) == NULL);
    assert(get_city_pv(city1) == 10);
    assert(start_project(city1, 'G', pos));
    print_buildlist(city1->buildings);
    destroy_city(city1);
    return 0;
}
