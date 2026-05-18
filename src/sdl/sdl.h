#ifndef SDL
#define SDL

#include <SDL2/SDL.h>
#include "../map/map.h"
#include "../city/city.h"

typedef struct _Game Game;

SDL_Texture* load_sprite(SDL_Renderer* renderer, const char* filepath);
void run_game_sdl(Game * game);

#endif
