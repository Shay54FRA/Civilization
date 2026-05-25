#ifndef SDL_HUD_H
#define SDL_HUD_H

#include "../game/game.h"
#include "../unit/unit.h"
#include <SDL.h>

void draw_panneau_global(SDL_Renderer* renderer, Game* game);
void draw_panneau_message_action(SDL_Renderer* renderer, const char* message, int screenW);
void draw_panneau_guide_actions(SDL_Renderer* renderer, Game* game);
void draw_panneau_arbre_tech(SDL_Renderer* renderer, Game* game, int screenW, int screenH);

// Panneaux dynamiques en bas de l'écran
void draw_panneau_biome_flottant(SDL_Renderer* renderer, Game* game, Position selection);
void draw_panneau_tuile_illuminee(SDL_Renderer* renderer, Game* game, Position selection, int screenW, int screenH, Unit * selected_unit);

void get_move_message(MoveResult result, char* buffer, size_t size);

#endif
