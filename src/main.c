#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "unit/unit.h"
#include "configuration/configuration.h"

char * minuscule(char *chaine)
{
    int i = 0;
    for (i = 0 ; chaine[i] != '\0' ; i++)
    {
        chaine[i] = tolower(chaine[i]);
    }

    return chaine;
}

int main(int argc, char *argv[]) {

    // VALEURS PAR DEFAUT
    int w = 50;
    int h = 30;
    unsigned int s= (unsigned int)time(NULL);
    int t = 200; //nbr tours
    int b = 3; //nbr barbares
    int is_sdl = 0; //CLI par défaut (mode texte)

    // PARSING
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--mode") == 0 && i + 1 < argc) { // si l'argument i est --mode  et qu'il y a bien quelque chose après l'option :

            if (strcmp(minuscule(argv[i+1]), "sdl") == 0) is_sdl = 1; // si l'argument d'après est SDL
        }
        else if (strcmp(argv[i], "--width") == 0 && i + 1 < argc) {
            w = atoi(argv[i+1]); // arguments arrivent sous forme de texte "40" -> atoi transforme ASCII (texte) en integer
        }
        else if (strcmp(argv[i], "--height") == 0 && i + 1 < argc) {
            h = atoi(argv[i+1]); 
        }
        else if (strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            s = (unsigned int)atoi(argv[i+1]); //nombre forcément positif
        }
        else if (strcmp(argv[i], "--turns") == 0 && i + 1 < argc){
            t = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i],"--barbarians") == 0 && i + 1 < argc){
            b = atoi(argv[i+1]);
        }
    }

    // CONFIGURATION
    Configuration* config = create_configuration(w, h, s,t,b, is_sdl);

    printf("Civ lance - Graine: %u | Taille: %dx%d | Mode: %s\n | Nbr tours : %d | Nbr barbares : %d", 
            config->s, config->w, config->h, config->is_sdl ? "SDL" : "CLI",config->t,config->b);

    destroy_configuration(config);
    
    return 0;
}
