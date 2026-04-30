#include <stdio.h>
#include <stdlib.h>

#include "configuration.h"

Configuration * create_configuration(int width, int height, unsigned int seed,int nbr_tours, int nbr_barbares, int sdl_mode) {

    Configuration * config = malloc(sizeof(Configuration));

    if(config == NULL){
        return NULL;
    }

    config->w = width;
    config->h = height;
    config->s = seed;
    config->t = nbr_tours;
    config->b = nbr_barbares;
    config->is_sdl = sdl_mode;

    return config;
}

void destroy_configuration(Configuration * config){
    free(config);
}
