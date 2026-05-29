#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <SDL.h>

#include "unit/unit.h"
#include "configuration/configuration.h"
#include "game/game.h"
#include "cli/cli.h"
#include "sdl/sdl.h"

char * minuscule(char *chaine)
{
    int i = 0;
    for (i = 0 ; chaine[i] != '\0' ; i++)
    {
        chaine[i] = tolower(chaine[i]);
    }

    return chaine;
}


void run_game_cli(Game * game);
void run_game_sdl(Game * game);


int main(int argc, char *argv[]) {

    // VALEURS PAR DEFAUT
    int w = 50;
    int h = 30;
    unsigned int s= (unsigned int)time(NULL);
    int t = 200; //nbr tours
    int b = 3; //nbr camps barbares
    int is_sdl = 0; //CLI par défaut (mode texte)

    // PARSING
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "--mode") == 0 || strcmp(argv[i], "-m") == 0) && i + 1 < argc) { // si l'argument i est --mode  et qu'il y a bien quelque chose après l'option :

            if (strcmp(minuscule(argv[i+1]), "sdl") == 0) is_sdl = 1; // si l'argument d'après est SDL
        }
        else if ((strcmp(argv[i], "--width") == 0 || strcmp(argv[i], "-W") == 0) && i + 1 < argc) {
            w = atoi(argv[i+1]); // arguments arrivent sous forme de texte "40" -> atoi transforme ASCII (texte) en integer
        }
        else if ((strcmp(argv[i], "--height") == 0 || strcmp(argv[i], "-H") == 0) && i + 1 < argc) {
            h = atoi(argv[i+1]); 
        }
        else if ((strcmp(argv[i], "--seed") == 0 || strcmp(argv[i], "-s") == 0) && i + 1 < argc) {
            s = (unsigned int)atoi(argv[i+1]); //nombre forcément positif
        }
        else if ((strcmp(argv[i], "--turns") == 0 || strcmp(argv[i], "-t") == 0) && i + 1 < argc){
            t = atoi(argv[i+1]);
        }
        else if ((strcmp(argv[i],"--barbarians") == 0 || strcmp(argv[i], "-b") == 0) && i + 1 < argc){
            b = atoi(argv[i+1]);
        }
    }

    // CONFIGURATION
    Configuration* config = create_configuration(w, h, s,t,b, is_sdl);

    printf("Civ lance - Graine: %u | Taille: %dx%d | Mode: %s\n | Nbr tours : %d | Nbr camps barbares : %d\n", 
            config->s, config->w, config->h, config->is_sdl ? "SDL" : "CLI",config->t,config->b);

    // GAME 
    Game * game = create_game(config);

    if(game == NULL){
        printf("Erreur : Impossible de créer la partie.");
        return 1;
    }

    // CHOIX DU MODE DE JEU
    if(config->is_sdl){ //si is_sdl == 1
        run_game_sdl(game);
    }
    else{
        run_game_cli(game);
    }

    // NETTOYAGE
    destroy_game(game);
    
    return 0;
}
