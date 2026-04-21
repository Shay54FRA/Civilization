
#ifndef BUILDING
#define BUILDING

typedef struct _Building {
    char type;
    int x;
    int y;
} Building;

typedef struct _BuildList { 
    Building* data;
    struct _BuildList* next;
} BuildList;

void get_building_type(Building* build);
void get_building_x(Building* build);
void get_building_y(Building* build);
void get_building_city(Building* build);

#endif
