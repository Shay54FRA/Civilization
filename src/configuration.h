#ifndef configuration

#define configuration
typedef struct {
    int w;          //largeur
    int h;          // hauteur
    unsigned int s; // graine (entier forcément positif)
    int is_sdl;     // 1 si mode SDL, 0 si mode CLI
} Configuration;

// fonction permettant d'initialiser la configuration du jeu
void initialiser_configuration(Configuration *config, int width, int height, unsigned int seed, int sdl_mode);

#endif
