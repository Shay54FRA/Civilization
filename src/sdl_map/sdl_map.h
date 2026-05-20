#ifndef SDL_MAP_H
#define SDL_MAP_H

#include "../map/map.h"
#include "../game/game.h"
#include "../city/city.h"
#include <SDL.h>

City* find_city_at_position(Game* game, Position pos);
char get_building_type_at(Game* game, Position pos);
char get_project_building_at(Game* game, Position pos);
SDL_Color get_biome_color(char biome);
const char* get_biome_name(char biome);
void draw_hexagones(SDL_Renderer* renderer, int x, int y, int R, SDL_Color color);

void draw_map_sdl(SDL_Renderer* renderer, Game* game, int R, int h, Position position_actuelle, int cameraX, int cameraY, 
                  SDL_Texture* tex_ville, SDL_Texture* tex_ville_mur, SDL_Texture* tex_guerrier, SDL_Texture* tex_colon, 
                  SDL_Texture* tex_camp, SDL_Texture* tex_barbare, SDL_Texture* tex_bat_const);

Position position_hexagone(int mx, int my, int R, int h, int cameraX, int cameraY, Game* game);

#endif
