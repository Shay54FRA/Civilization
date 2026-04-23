
#ifndef BUILDING
#define BUILDING

typedef struct _Position Position;

typedef struct _Building {
    char type;
    Position* pos;
} Building;

typedef struct _BuildList { 
    Building* data;
    struct _BuildList* next;
} BuildList;

void get_building_type(Building* build);
void get_building_pos(Building* build);

#endif
