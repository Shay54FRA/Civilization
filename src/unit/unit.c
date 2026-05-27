#include "unit.h"
#include "../game/game.h"
#include "../tile/tile.h"
#include "../technology/technology.h"
#include "../building/building.h"
#include "../barbarian/barbarian.h"
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
    if (unit_list == NULL) return;
    if (unit_list->data != NULL) {
        destroy_unit(unit_list->data);
    }
    destroy_unit_list(unit_list->next);
    free(unit_list);
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

    if (!buildlist_contains(city->buildings, 'C'))
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

void kill_nth_unit(Game* game, int n) {
    if (game == NULL) return;
    if (get_unit_number(game) <= n) return;
    int current_unit_id = 0;
    UnitList* to_check = game->unitList;
    for (int ind = 0; ind < n; ind++) {
        to_check = to_check->next;
    }
    kill_unit(game, to_check->data);
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
            if (tile != NULL) {
                tile->unit = NULL;
            }
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

    if (dest_tile->unit != NULL)
        return MOVE_ALLY_OCCUPIED;

    /*
     * Les unités du joueur sont stockées directement dans les tuiles,
     * mais les barbares sont gérés dans une liste séparée.
     * Avant de faire un déplacement classique, on vérifie donc si la case cible
     * contient un barbare : dans ce cas, le déplacement devient une attaque.
     */
    Barbarian* barb = get_barbarian_at(game, dest);

    if (barb != NULL) {
        if (unit->atk <= 0)
            return MOVE_ATTACK_FORBIDDEN;

        int dmg_to_target = unit->atk - barb->def;
        if (dmg_to_target < 1)
            dmg_to_target = 1;

        int dmg_to_attacker = barb->atk - unit->def;
        if (dmg_to_attacker < 0)
            dmg_to_attacker = 0;

        barb->pv -= dmg_to_target;
        unit->pv -= dmg_to_attacker;

        // Attaquer consomme les points de mouvement nécessaires pour atteindre la case.
        unit->pm -= cost;

        /*
         * Si les deux combattants meurent, on les retire tous les deux.
         * L'unité ne peut évidemment pas occuper la case après le combat.
         */
        if (barb->pv <= 0 && unit->pv <= 0) {
            kill_barbarian(game, barb);
            kill_unit(game, unit);
            return MOVE_ATTACKER_DEAD;
        }

        // Si seule l'unité meurt, le barbare reste en place.
        if (unit->pv <= 0) {
            kill_unit(game, unit);
            return MOVE_ATTACKER_DEAD;
        }

        /*
         * Si le barbare meurt et que l'unité survit, l'unité prend sa place :
         * c'est le même comportement qu'un déplacement vers une case libérée.
         */
        if (barb->pv <= 0) {
            kill_barbarian(game, barb);

            Tile* current_tile = get_tile(game->map, unit->pos);
            if (current_tile) current_tile->unit = NULL;

            dest_tile->unit = unit;
            unit->pos = dest;

            return MOVE_COMBAT_WIN;
        }

        // Le barbare a survécu : l'unité a attaqué mais ne se déplace pas.
        return MOVE_COMBAT_BLOCKED;
    }

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
