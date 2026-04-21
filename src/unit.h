#ifndef UNIT
#define UNIT

typedef struct _Unit {
    char type;
    int atk;
    int def;
    int cost_per_turn;
    int pv;
    int pm;
} Unit;

#endif