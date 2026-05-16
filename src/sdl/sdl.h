#ifndef SDL
#define SDL

#include <SDL2/SDL.h>
#include "../map/map.h"

typedef struct _Game Game;


SDL_Color get_biome_color(char biome);
const char* get_biome_name(char biome);
void draw_hexagones(SDL_Renderer* renderer, int x, int y, int R, SDL_Color color);
void draw_map_sdl(SDL_Renderer* renderer, Game* game, int R, int h, Position position_actuelle, int cameraX, int cameraY);
void draw_panneau_global(SDL_Renderer* renderer, Game* game);
void draw_panneau_tuile_illuminee(SDL_Renderer* renderer, Game* game, Position selection, int screenW, int screenH);
Position position_hexagone(int mx, int my, int R, int h, int cameraX, int cameraY, Game* game);
void run_game_sdl(Game * game);

#endif
