#include "barbarian.h"
#include "../game/game.h"
#include "../unit/unit.h"
#include "../city/city.h"
#include "../map/map.h"
#include "../tile/tile.h"
#include "../configuration/configuration.h"

#include <stdlib.h>
#include <stdio.h>

Barbarian* create_barbarian(Position pos) {
    Barbarian* barb = malloc(sizeof(Barbarian));
    if (barb == NULL) return NULL;
    barb->pm = 3;
    barb->max_pm = 3;
    barb->pos = create_position(pos.x, pos.y);
    barb->pv = 15;
    barb->atk = 3;
    barb->def = 2;
    return barb;
}

void destroy_barbarian(Barbarian* barb) {
    if (barb != NULL) {
        destroy_position(barb->pos);
        free(barb);
    }
}

void kill_barbarian(Game* game, Barbarian* barb) {
    if (game == NULL || barb == NULL) return;
    BarbarianList* to_check = game->barbarianList;
    BarbarianList* previous = NULL;
    while (to_check != NULL) {
        if (to_check->data == barb) { // On a trouvé le barbare en question
            Tile* tile = get_tile(game->map, *(barb->pos));
            tile->barb_on = NULL;
            if (previous == NULL) {
                game->barbarianList = to_check->next;
            } else {
                previous->next = to_check->next;
            }
            game->barbs_number -= 1;
            destroy_barbarian(barb);
            free(to_check);
            break;
        }
        previous = to_check;
        to_check = to_check->next;
    }
}

BarbarianList* create_barb_list(Barbarian* barb, BarbarianList* next) {
    BarbarianList* barb_list = malloc(sizeof(BarbarianList));
    if (barb_list == NULL) return NULL;
    barb_list->data = barb;
    barb_list->next = next;
    return barb_list;
}

// Destruction des barbares avec
void destroy_barb_list(BarbarianList* barb_list) {
    if (barb_list == NULL) return;
    if (barb_list->data != NULL) {
        destroy_barbarian(barb_list->data);
    }
    destroy_barb_list(barb_list->next);
    free(barb_list);
}

int get_barb_pv(Barbarian* barb){
    if (barb == NULL) return -1;
    return barb->pv;
}
int get_barb_pm(Barbarian* barb) {
    if (barb == NULL) return -1;
    return barb->pm;
}

Position* get_barb_pos(Barbarian* barb) {
    if (barb == NULL) return NULL;
    return barb->pos;
}

Position get_nearest_target(Game* game, Barbarian* barb) {
    Position rep = {-1, -1};
    if (game == NULL || barb == NULL) return rep;
    if (game->map == NULL) return rep;
    Position barb_pos = *(get_barb_pos(barb));
    int min_dist = game->map->height * game->map->length; //Majorant de la distance max
    UnitList* unit_to_check = game->unitList;
    Unit* unit;
    int dist;
    while (unit_to_check != NULL) {
        unit = unit_to_check->data;
        if (unit != NULL) {
            dist = get_distance(barb_pos, unit->pos);
            if (dist < min_dist) {
                min_dist = dist;
                rep = unit->pos;
            }
        }
        unit_to_check = unit_to_check->next;
    }
    CityList* city_to_check = game->cityList;
    City* city;
    while (city_to_check != NULL) {
        city = city_to_check->city;
        if (city != NULL) {
            dist = get_distance_to_city(city, barb_pos);
            if (dist < min_dist) {
                min_dist = dist;
                rep = city->pos;
            }
        }
        city_to_check = city_to_check->next;
    }
    return rep;
}

void move_barbarian(Game* game, Barbarian* barb, Position pos_cible) {
    if (game == NULL || barb == NULL || pos_cible.x < 0 || pos_cible.y < 0) return;
    int to_reach = get_distance(pos_cible, *(get_barb_pos(barb)));
    /* Idée 1 : tester le déplacement dans toute les directions */
    int test_dist;
    while (get_barb_pm(barb) > 0) {
        TileList* neighbors = get_exploited_tiles(game->map, get_tile(game->map, *(get_barb_pos(barb))), 1);
        TileList* move_to_check = neighbors;
        while (move_to_check != NULL) {
            Tile* tile = move_to_check->data;
            if (tile == NULL) {
                move_to_check = move_to_check->next;
                break;
            }
            Position pos = tile->pos;
            test_dist = get_distance(pos_cible, pos);
            if (test_dist < to_reach) { // = On se rapproche
                if (get_barb_pm(barb) >= get_terrain_cost(tile->biome)) { // = On peut passer ce terrain (pas le plus efficient)
                    barb->pm -= get_terrain_cost(tile->biome);

                    //CAS 1 : Combat
                    if (test_dist == 0) {
                        int fight = barbarian_attack(game, barb, pos);
                        if (fight == -1 || fight == 0) {// Le barbare est mort ou bien l'entité est tjrs vivante
                            return;
                        } else { // Le barbare à gagné
                            Tile* old_tile = get_tile(game->map, *(barb->pos));
                            old_tile->barb_on = NULL;
                            barb->pos->x = pos.x;
                            barb->pos->y = pos.y;
                            tile->barb_on = barb;
                            break;
                        }
                    }

                    //CAS 2 : Pas combat -> déplacement libre ou camp/barbare
                    else if (tile->barb_on == NULL && !(tile->camp_on)) {
                        Tile* old_tile = get_tile(game->map, *(barb->pos));
                        old_tile->barb_on = NULL;
                        barb->pos->x = pos.x;
                        barb->pos->y = pos.y;
                        tile->barb_on = barb;
                        break;
                    } 
                }
            }
            move_to_check = move_to_check->next;
        }
        destroy_tilelist(neighbors);
        if (move_to_check == NULL) {
            break;
        }
    }
}

int barbarian_attack(Game* game, Barbarian* barb, Position pos) {
    if (game == NULL || barb == NULL) return -1;
    Tile* fighting_tile = get_tile(game->map, pos);

    //Cas 1 : On affronte une unité
    Unit* unit = fighting_tile->unit;
    if (fighting_tile->unit != NULL) {
        int dmg_to_target = barb->atk - unit->def;
        if (dmg_to_target < 1) dmg_to_target = 1;

        int dmg_to_attacker = unit->atk - barb->def;
        if (dmg_to_attacker < 0) dmg_to_attacker = 0;

        unit->pv -= dmg_to_target;
        barb->pv -= dmg_to_attacker;

        if (barb->pv <= 0) {
            kill_barbarian(game, barb);
            return -1;
        }

        if (unit->pv <= 0) {
            kill_unit(game, unit);
            return 1;
        }
        return 0;
    }

    // Cas 2 : On affronte une ville
    City* city = get_city_on_tile(game->cityList, fighting_tile);
    if (city != NULL) {
        int dmg_to_target = barb->atk - CITY_STRENGTH;
        if (dmg_to_target < 1) dmg_to_target = 1;

        int dmg_to_attacker = CITY_STRENGTH - barb->def;
        if (dmg_to_attacker < 0) dmg_to_attacker = 0;

        city->damage += dmg_to_target;
        city->has_taken_damage = true;
        barb->pv -= dmg_to_attacker;

        if (barb->pv <= 0) {
            kill_barbarian(game, barb);
            return -1;
        }

        if (get_city_pv(city) <= 0) {
            kill_city(game, city);
            return 1;
        }
        return 0;
    }
    // Cas erreur
    return -1;
}

void move_all_barbarians(Game* game) {
    if (game == NULL) return;
    BarbarianList* barb_list = game->barbarianList;
    while (barb_list != NULL) {
        if (barb_list->data != NULL) {
            Barbarian* barb = barb_list->data;
            Position target_pos = get_nearest_target(game, barb);
            move_barbarian(game, barb, target_pos);
        }
        barb_list = barb_list->next;
    }
}

int destroy_camp(Game* game, Position pos) {
    if (game == NULL) return 0;
    if (game->map == NULL) return 0;
    int camps_not_destroyed = get_camp_list_number(game) - 1; //On retire déjà celui qui sera compté dans la première boucle et retiré après
    Tile* tile_with_camp = get_tile(game->map, pos);
    if (!(tile_with_camp->camp_on)) return 0;
    tile_with_camp->camp_on = false; // Destruction du camp
    return 5*(game->configuration->b - camps_not_destroyed); // = 5 * (nbr_camps_max - nbr_camps_restants)
}

void spawn_all_barbarians(Game* game) {
    if (game == NULL) return;
    for (int x = 0; x < game->map->length; x++) {
        for (int y = 0; y < game->map->height; y++) {
            Position pos = {x,y};
            Tile* tile = get_tile(game->map, pos);
            if (tile->camp_on && (tile->barb_on == NULL) && (tile->unit == NULL) && (game->barbs_number <= MAX_BARBS)) {
                game->barbs_number++;
                Barbarian* new_barb = create_barbarian(pos);
                game->barbarianList = create_barb_list(new_barb, game->barbarianList); // On le met au début de la liste
            }
        }
    }
}

void reset_all_barbs_pm(BarbarianList* barb_list) {
    BarbarianList* to_check = barb_list;
    while (to_check != NULL) {
        if (to_check->data != NULL) {//Le barbare existe
            to_check->data->pm = to_check->data->max_pm;
        }
        to_check = to_check->next;
    }
}