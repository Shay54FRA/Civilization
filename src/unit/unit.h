#ifndef UNIT
#define UNIT

typedef struct _Position Position;

typedef struct _Unit {
    char type;
    int atk;
    int def;
    int cost_per_turn; //Nom trop long
    int pv;
    int pm;
    Position* pos;
} Unit;

typedef struct _UnitList { //Liste chaîné pour regrouper toutes les unités de la partie
    Unit* data;
    struct _UnitList* next;
} UnitList;

Unit* create_unit(void);
void destroy_unit(Unit* unit);

int get_atk(Unit* unit);
int get_pv(Unit* unit);
int get_def(Unit* unit);
int get_pm(Unit* unit);
int get_cost_per_turn(Unit* unit);
char get_unit_type(Unit* unit);
Position get_unit_pos(Unit* unit);

Unit* get_unit(UnitList* lst);
UnitList* get_unit_next(UnitList* lst); //Prbl de logique, restructuration des lst chaînées ?

#endif
