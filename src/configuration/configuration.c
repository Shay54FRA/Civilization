#include <stdio.h>
#include <stdlib.h>

#include "configuration.h"

Configuration * create_configuration(int width, int height, unsigned int seed,int nbr_tours, int nbr__camps_barbares, int sdl_mode) {

    Configuration * config = malloc(sizeof(Configuration));

    if(config == NULL){
        return NULL;
    }

    config->w = width;
    config->h = height;
    config->s = seed;
    config->t = nbr_tours;
    config->b = nbr_camps_barbares;
    config->is_sdl = sdl_mode;

    return config;
}

int get_width(Configuration * config){
    return config->w;
}

int get_height(Configuration * config){
    return config->h;
}

unsigned int get_seed(Configuration * config){
    return config->s;
}

int get_nbr_tours(Configuration * config){
    return config->t;
}

int get_nbr_camps_barbares(Configuration * config){
    return config->b;
}

int get_mode(Configuration * config){
    return config->is_sdl;
}

void destroy_configuration(Configuration * config){
    free(config);
}
