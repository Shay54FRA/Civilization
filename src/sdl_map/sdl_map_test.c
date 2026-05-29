#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <SDL.h>

#include "../src/sdl_map/sdl_map.h"
#include "../src/game/game.h"
#include "../src/map/map.h"
#include "../src/tile/tile.h"

//test traduction des biomes
void test_biomes_logic(void) {

    // Vérification des noms textuels
    assert(strcmp(get_biome_name('E'), "Eau") == 0);
    assert(strcmp(get_biome_name('P'), "Plaine") == 0);
    assert(strcmp(get_biome_name('F'), "Foret") == 0);
    assert(strcmp(get_biome_name('M'), "Montagne") == 0);
    assert(strcmp(get_biome_name('D'), "Desert") == 0);
    assert(strcmp(get_biome_name('T'), "Toundra") == 0);
    assert(strcmp(get_biome_name('X'), "Inconnu") == 0); // Cas de base / Erreur

    // Vérification des couleurs
    
    // Eau ('E')
    SDL_Color eau_c = get_biome_color('E');
    assert(eau_c.r == 0 && eau_c.g == 105 && eau_c.b == 148 && eau_c.a == 255);

    // Plaine ('P')
    SDL_Color plaine_c = get_biome_color('P');
    assert(plaine_c.r == 34 && plaine_c.g == 139 && plaine_c.b == 34 && plaine_c.a == 255);

    // Forêt ('F')
    SDL_Color foret_c = get_biome_color('F');
    assert(foret_c.r == 0 && foret_c.g == 100 && foret_c.b == 0 && foret_c.a == 255);

    // Montagne ('M')
    SDL_Color montagne_c = get_biome_color('M');
    assert(montagne_c.r == 128 && montagne_c.g == 128 && montagne_c.b == 128 && montagne_c.a == 255);

    // Désert ('D')
    SDL_Color desert_c = get_biome_color('D');
    assert(desert_c.r == 194 && desert_c.g == 178 && desert_c.b == 128 && desert_c.a == 255);

    // Toundra ('T')
    SDL_Color toundra_c = get_biome_color('T');
    assert(toundra_c.r == 210 && toundra_c.g == 210 && toundra_c.b == 210 && toundra_c.a == 255);

    // Cas de base par défaut (on prend ici 'X' mais ça aurait pu être n'importe quel autre caractère)
    SDL_Color def_c = get_biome_color('X');
    assert(def_c.r == 255 && def_c.g == 255 && def_c.b == 255 && def_c.a == 255);
}

// Test de la détection du clic
void test_position_hexagone_math(void) {
    // On alloue une structure Game et Map juste pour le test
    Game* game = malloc(sizeof(Game));
    game->map = malloc(sizeof(Map));
    game->map->height = 2;
    game->map->length = 2;

    int R = 60;
    int h = 51; // (int)(60 * 0.866)
    int camX = 0;
    int camY = 0;

    // Simulation d'un clic de souris au centre théorique de la case (j=0, i=0)
    // Formule du code : x = j * (2 * h) + ((i % 2) * h) - cameraX + (1280 / 2);
    // Pour (0,0) -> x = 0 * 102 + 0 - 0 + 640 = 640
    // Pour (0,0) -> y = 0 * 90 - 0 + 360 = 360
    int mouse_center_x = 640;
    int mouse_center_y = 360;

    Position p_exacte = position_hexagone(mouse_center_x, mouse_center_y, R, h, camX, camY, game);
    assert(p_exacte.x == 0 && p_exacte.y == 0);

    // Simulation d'un clic légèrement décentré mais toujours dans le rayon de la même case
    Position p_decalee = position_hexagone(650, 370, R, h, camX, camY, game);
    assert(p_decalee.x == 0 && p_decalee.y == 0);

    // Test du garde-fou : un clic dans le grand vide noir (on test ici tout en haut à gauche de l'écran pixel 0,0)
    // Doit renvoyer (-1, -1) car la distance carrée dépasse R*R
    Position p_vide = position_hexagone(0, 0, R, h, camX, camY, game);
    assert(p_vide.x == -1 && p_vide.y == -1);

    // Libération de notre structure simulée
    free(game->map);
    free(game);
}

int main(void) {
    printf("Demarrage des tests unitaires logiques pour sdl_map...\n");
    
    test_biomes_logic();
    test_position_hexagone_math();

    printf("=> TOUS LES TESTS DE LA CARTE ONT REUSSI ! (17/17 assertions OK)\n");
    return 0;
}
