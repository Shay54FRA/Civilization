#ifndef CONFIGURATION
#define CONFIGURATION

typedef struct _Configuration {
    int w;          //largeur
    int h;          // hauteur
    unsigned int s; // graine (entier forcément positif)
    int t;
    int b;
    int is_sdl;     // 1 si mode SDL, 0 si mode CLI
} Configuration;

// fonction permettant d'initialiser la configuration du jeu
Configuration * create_configuration(int width, int height, unsigned int seed,int nbr_tours,int nbr_camps_barabares, int sdl_mode);
void destroy_configuration(Configuration * config);
int get_width(Configuration * config);
int get_height(Configuration * config);
unsigned int get_seed(Configuration * config);
int get_nbr_tours(Configuration * config);
int get_nbr_camps_barbares(Configuration * config);
int get_mode(Configuration * config);
void destroy_configuration(Configuration * config);

#endif
