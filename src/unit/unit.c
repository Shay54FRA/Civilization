#include "unit.h"
#include <stdio.h>
#include <stdlib.h>

void destroy_unit(Unit* unit) {
    if (unit != NULL) {
        free(unit);
    }
}