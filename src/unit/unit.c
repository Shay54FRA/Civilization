#include "unit.h"
#include "../game/game.h"
#include "../tile/tile.h"
#include "../technology/technology.h"
#include "../building/building.h"
#include <stdlib.h>

int get_terrain_cost(char biome)
{
    if (biome == 'E') return 999;
    if (biome == 'F' || biome == 'M') return 2;
    return 1;
}

Unit* create_unit(char type, Position pos)
{
    Unit* u = malloc(sizeof(Unit));
    if (!u) return NULL;

    u->type = type;
    u->pos = pos;

    if (type == 'c') { // Colon
        u->max_pv = 10;
        u->pv = 10;
        u->atk = 0;
        u->def = 1;
        u->max_pm = 2;
        u->pm = 2;
        u->cost_per_turn = 0;
    }
    else if (type == 'g') { // Guerrier
        u->max_pv = 15;
        u->pv = 15;
        u->atk = 3;
        u->def = 2;
        u->max_pm = 3;
        u->pm = 3;
        u->cost_per_turn = 1;
    }
    else {
        u->max_pv = 10;
        u->pv = 10;
        u->atk = 1;
        u->def = 1;
        u->max_pm = 2;
        u->pm = 2;
        u->cost_per_turn = 0;
    }

    return u;
}

void destroy_unit(Unit* unit)
{
    if (!unit) return;
    free(unit);
}

void destroy_unit_list(UnitList* unit_list) {
    UnitList* to_check = unit_list;
    while (to_check != NULL) {
        if (to_check->data != NULL) {
            destroy_unit(to_check->data);
        }
        destroy_unit_list(to_check->next);
        to_check = to_check->next;
        free(to_check);
    }
}

int get_atk(Unit* unit) { return unit ? unit->atk : 0; }
int get_pv(Unit* unit) { return unit ? unit->pv : 0; }
int get_def(Unit* unit) { return unit ? unit->def : 0; }
int get_pm(Unit* unit) { return unit ? unit->pm : 0; }
int get_cost_per_turn(Unit* unit) { return unit ? unit->cost_per_turn : 0; }
char get_unit_type(Unit* unit) { return unit ? unit->type : '\0'; }

Position get_unit_pos(Unit* unit)
{
    Position empty = {-1, -1};
    return unit ? unit->pos : empty;
}

Unit* get_unit(UnitList* lst)
{
    return lst ? lst->data : NULL;
}

UnitList* get_unit_next(UnitList* lst)
{
    return lst ? lst->next : NULL;
}

void spawn_unit_from_project(Game* game, City* city)
{
    if (!game || !game->map || !city || !city->project) return;

    char p_type = city->project->type;

    if (p_type != 'c' && p_type != 'g')
        return;

    if (p_type == 'g' && !is_unit_unlocked(game->tech_tree, 'g'))
        return;

    if (p_type == 'g' && !buildlist_contains(city->buildings, 'C'))
        return;

    Tile* tile = get_tile(game->map, city->project->pos);
    if (!tile || tile->unit != NULL)
        return;

    Unit* new_unit = create_unit(p_type, city->project->pos);
    if (!new_unit) return;

    if (game->tech_tree) {
        new_unit->max_pm += game->tech_tree->bonus_pm_units;
        new_unit->pm = new_unit->max_pm;
    }

    UnitList* node = malloc(sizeof(UnitList));
    if (!node) {
        destroy_unit(new_unit);
        return;
    }

    node->data = new_unit;
    node->next = game->unitList;
    game->unitList = node;

    tile->unit = new_unit;
}

void resolve_combat(Game* game, Unit* attacker, Unit* target, Tile* target_tile)
{
    if (!game || !attacker || !target || !target_tile) return;
    if (attacker->atk == 0) return;

    int dmg_to_target = attacker->atk - target->def;
    if (dmg_to_target < 1) dmg_to_target = 1;

    int dmg_to_attacker = target->atk - attacker->def;
    if (dmg_to_attacker < 0) dmg_to_attacker = 0;

    target->pv -= dmg_to_target;
    attacker->pv -= dmg_to_attacker;

    if (target->pv <= 0) {
        target_tile->unit = NULL;
    }

    if (attacker->pv <= 0) {
        Tile* attacker_tile = get_tile(game->map, attacker->pos);
        if (attacker_tile) attacker_tile->unit = NULL;
    }
}

void kill_unit(Game* game, Unit* unit) {
    if (unit == NULL) return;
    if (game == NULL) destroy_unit(unit); return;
    UnitList* to_check = game->unitList;
    UnitList* previous = NULL;
    while (to_check != NULL) {
        Unit* test_unit = to_check->data;
        if (test_unit == unit) {
            if (previous == NULL) {
                game->unitList = to_check->next;
            } else {
                previous->next = to_check->next;
            }
            Tile* tile = get_tile(game->map, unit->pos);
            tile->unit = NULL;
            destroy_unit(unit);
            free(to_check);
            break;
        }
        previous = to_check;
        to_check = to_check->next;
    }
}

MoveResult move_unit_step(Game* game, Unit* unit, Position dest)
{
    if (!game || !game->map || !unit)
        return MOVE_NO_UNIT;

    if (unit->pm <= 0)
        return MOVE_NO_PM;

    Tile* dest_tile = get_tile(game->map, dest);
    if (!dest_tile)
        return MOVE_INVALID_TILE;

    int dx = dest.x - unit->pos.x;
    int dy = dest.y - unit->pos.y;

    if (dx < -1 || dx > 1 || dy < -1 || dy > 1)
        return MOVE_NOT_ADJACENT;

    if (dx == 0 && dy == 0)
        return MOVE_INVALID_TILE;

    if (dest_tile->biome == 'E')
        return MOVE_WATER;

    int cost = get_terrain_cost(dest_tile->biome);

    if (unit->pm < cost)
        return MOVE_NOT_ENOUGH_PM;

    /*
     * Ici, dest_tile->unit représente une unité du joueur.
     * Les barbares sont dans leur propre module, donc on ne combat pas ici.
     */
    if (dest_tile->unit != NULL)
        return MOVE_ALLY_OCCUPIED;

    Tile* current_tile = get_tile(game->map, unit->pos);
    if (current_tile) current_tile->unit = NULL;

    dest_tile->unit = unit;
    unit->pos = dest;
    unit->pm -= cost;

    return MOVE_OK;
}

void reset_all_pm(UnitList* lst)
{
    UnitList* current = lst;

    while (current != NULL) {
        if (current->data != NULL) {
            current->data->pm = current->data->max_pm;
        }

        current = current->next;
    }
}
