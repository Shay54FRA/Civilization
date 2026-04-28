#include "map.h"
#include <stdlib.h>
#include <stdio.h>


Map* create_map(int width, int height, int seed){

    srand(seed); //Decide de la génération aléatoire selon la seed donnée. (Permet de rejouer la partie)

    Map* m = malloc(sizeof(Map));
    m->height=height;
    m->length=width;
    m->map=malloc(sizeof(Tile**)*height);

    for(int i=0;i<height;i=i+1){
        m->map[i]=malloc(sizeof(Tile*)*width);

        for(int j=0;j<width;j=j+1){
            m->map[i][j] = malloc(sizeof(Tile));
            m->map[i][j]->pos = malloc(sizeof(Position));
            m->map[i][j]->pos->x = j; // X = Colonnes (largeur)
            m->map[i][j]->pos->y = i; // Y = Lignes (hauteur)
            m->map[i][j]->city_on = NULL;
            m->map[i][j]->unit = NULL;

            //Initialisation biome
            int alea = rand() % 100;
            if (alea < 40) {
                m->map[i][j]->biome = 'P'; // 40% de chance d'avoir une Plaine
            }
            else if (alea < 50) {
                m->map[i][j]->biome = 'F'; // 10% de chance d'avoir une Forêt
            }
            else if (alea < 60) {
                m->map[i][j]->biome = 'M'; // 10% de chance d'avoir une Montagne
            }
            else if (alea < 70) {
                m->map[i][j]->biome = 'E'; // 10% de chance d'avoir de l'Eau
            }
            else if (alea < 80) {
                m->map[i][j]->biome = 'D'; // 10% de chance d'avoir un Désert
            }
            else{
                m->map[i][j]->biome = 'T'; // 10% de chance d'avoir une Toundra
            }
        }
    }

    return m;
}


void destroy_map(Map* m) {
    if (m != NULL) {
        for (int i = 0; i < m->height; i++) {
            for(int j=0; j< m->length;j++){
                free(m->map[i][j]->pos);
                free(m->map[i][j]);
            }
            free(m->map[i]); 
        }

        free(m->map);
        
        free(m);
    }
}


// Définition des couleurs ANSI
#define COLOR_RESET    "\x1b[0m"
#define COLOR_EAU      "\x1b[38;5;33m"   // Bleu plus clair/océan
#define COLOR_PLAINE   "\x1b[38;5;114m"  // Vert tendre
#define COLOR_FORET    "\x1b[38;5;22m"   // Vert sapin très sombre
#define COLOR_MONTAGNE "\x1b[38;5;244m"  // Gris roche
#define COLOR_DESERT   "\x1b[38;5;220m"  // Jaune sable
#define COLOR_TOUNDRA  "\x1b[38;5;159m"  // Bleu givré glacé

void print_map(Map* m) {
    if (m == NULL || m->map == NULL) return;

    for (int i = 0; i < m->height; i++) {
        
        for (int j = 0; j < m->length; j++) {
            
            Tile* current_tile = m->map[i][j];


            switch(current_tile->biome) {
                case 'E': printf("%s ~ %s", COLOR_EAU, COLOR_RESET); break;
                case 'P': printf("%s . %s", COLOR_PLAINE, COLOR_RESET); break;
                case 'F': printf("%s # %s", COLOR_FORET, COLOR_RESET); break;
                case 'M': printf("%s ^ %s", COLOR_MONTAGNE, COLOR_RESET); break;
                case 'D': printf("%s x %s", COLOR_DESERT, COLOR_RESET); break;
                case 'T': printf("%s * %s", COLOR_TOUNDRA, COLOR_RESET); break;
                default:  printf(" ? "); break; // Sécurité si un biome inconnu s'est glissé
            }

        }
        printf("\n");
    }
}
