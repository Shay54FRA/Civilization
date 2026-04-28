#ifndef BARBARIAN
#define BARBARIAN

#include "../map/map.h"
#include <stdbool.h>

typedef struct _Barbarian {
    int pv;
    int pm;
    Position* pos;
} Barbarian;

typedef struct _BarbarianList {
    Barbarian* data;
    struct _Barbarian* next;
} BarbarianList;

int get_barb_pv(Barbarian barb);
int get_barb_pm(Barbarian barb);
int get_barb_pos(Barbarian barb);

typedef struct _Camp {
    Position* pos;
    bool occupied; //Dit si la case est occupé (pour la génération de troupe)
} Camp;

typedef struct _CampList {
    Camp* data;
    struct _CampList* next;
} CampList;

Camp* get_camplist_data(CampList* lst);
CampList* get_camplist_next(CampList* lst);


#endif
