#include <stdio.h>
#include <stdlib.h>

#include "configuration.h"

void initialiser_configuration(Configuration *config, int width, int height, unsigned int seed, int sdl_mode) {
    config->w = width;
    config->h = height;
    config->s = seed;
    config->is_sdl = sdl_mode;
}
