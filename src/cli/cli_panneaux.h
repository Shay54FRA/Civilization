#ifndef CLI_PANNEAUX_H
#define CLI_PANNEAUX_H

#include "../game/game.h"
#include "../unit/unit.h"
#include "../map/map.h"


void print_tile_info(WINDOW* win,Game* game, Position cursor);
void print_selected_unit_info(WINDOW* win,Unit* selected_unit);
void show_city_info_cli(WINDOW* win,Game* game, Position pos);


void print_stats(WINDOW* win,Game* game);


void print_action_help(WINDOW* win);

#endif
