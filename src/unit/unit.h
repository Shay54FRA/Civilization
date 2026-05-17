#ifndef UNIT
#define UNIT

#include <stdbool.h>
#include "../map/map.h"
#include "../city/city.h"

typedef enum {
    MOVE_OK,
    MOVE_NO_UNIT,
    MOVE_NO_PM,
    MOVE_INVALID_TILE,
    MOVE_WATER,
    MOVE_NOT_ADJACENT,
    MOVE_NOT_ENOUGH_PM,
    MOVE_ALLY_OCCUPIED,
    MOVE_COMBAT_WIN,
    MOVE_COMBAT_BLOCKED,
    MOVE_ATTACKER_DEAD,
    MOVE_ATTACK_FORBIDDEN
} MoveResult;

typedef struct _Position Position;
typedef struct _Camp Camp;
typedef struct _Game Game;

typedef struct _Unit {
    char type;

    int atk;
    int def;
    int cost_per_turn;

    int pv;
    int max_pv;

    int pm;
    int max_pm;

    Position pos;
} Unit;

typedef struct _UnitList {
    Unit* data;
    struct _UnitList* next;
} UnitList;

Unit* create_unit(char type, Position pos);
void destroy_unit(Unit* unit);

int get_atk(Unit* unit);
int get_pv(Unit* unit);
int get_def(Unit* unit);
int get_pm(Unit* unit);
int get_cost_per_turn(Unit* unit);
char get_unit_type(Unit* unit);
Position get_unit_pos(Unit* unit);

//Vérifier la condition de distance, créer la ville et l'ajouter à game
void colonize(Game* game, Unit* colon);

//La détruit tout en la retirant de la partie
void kill_unit(Game* game, Unit* unit);


Unit* get_unit(UnitList* lst);
UnitList* get_unit_next(UnitList* lst);

void spawn_unit_from_project(Game* game, City* city);
MoveResult move_unit_step(Game* game, Unit* unit, Position dest);
void resolve_combat(Game* game, Unit* attacker, Unit* target, Tile* target_tile);
void reset_all_pm(UnitList* lst);
int get_terrain_cost(char biome);

#endif
