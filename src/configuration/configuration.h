#ifndef configuration

#define configuration
typedef struct {
    int w;          //largeur
    int h;          // hauteur
    unsigned int s; // graine (entier forcément positif)
    int t;
    int b;
    int is_sdl;     // 1 si mode SDL, 0 si mode CLI
} Configuration;

// fonction permettant d'initialiser la configuration du jeu
Configuration * create_configuration(int width, int height, unsigned int seed,int nbr_tours,int nbr_barabares, int sdl_mode);
void destroy_configuration(Configuration * config);

#endif
