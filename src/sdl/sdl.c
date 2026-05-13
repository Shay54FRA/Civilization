#include "../map/map.h"
#include "../tile/tile.h"
#include "../game/game.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

void run_game_sdl(Game * game){

    //INITIALISATION DE SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in init : %s", SDL_GetError());
        return;
    }

    int width = 1280;
    int height = 720;

    //CREATION WINDOW
    SDL_Window * window = SDL_CreateWindow("Civilisation du groupe PP2ix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_SHOWN);

    if (window == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in window init: %s", SDL_GetError());
        SDL_Quit();
        return;
    }

    //CREATION RENDERER
    SDL_Renderer * renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    if(renderer == NULL){
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Error in renderer init: %s",SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    //BOUCLE DE JEU
    int running = 1;
    SDL_Event event;
    
    while(running){
        /* Poll Event récupère les évents en attente */
        if(SDL_PollEvent(&event)){ 
            switch(event.type){
                case SDL_QUIT:
                    running = 0;
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer); 
        //rempli la surface gérée par le moteur graphique avec la couleur courante

        SDL_RenderPresent(renderer);


    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}