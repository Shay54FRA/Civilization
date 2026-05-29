#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/sdl_panneaux/sdl_panneaux.h"
#include "../src/game/game.h" // Pour récupérer la définition de MoveResult

void test_get_move_message_all_cases(void) {
    char buf[100]; // buffer vide = feuille blanche

    //Test d'un déplacement normal
    get_move_message(MOVE_OK, buf, sizeof(buf)); 
    assert(strcmp(buf, "Deplacement effectue !") == 0);

    // Test quand on a pas sélectionné une unité
    get_move_message(MOVE_NO_UNIT, buf, sizeof(buf)); // On regarde si avec l'erreur MOVE_NO_UNIT, on a le bon message affiché
    assert(strcmp(buf, "Aucune unite selectionnee") == 0);

    // Test quand on veut déplacer une unité alors qu'elle n'a plus de PM
    get_move_message(MOVE_NO_PM, buf, sizeof(buf));
    assert(strcmp(buf, "Cette unite n'a plus de PM") == 0);

    // Test quand on veut déplacer sur une case invalide
    get_move_message(MOVE_INVALID_TILE, buf, sizeof(buf));
    assert(strcmp(buf, "Case invalide") == 0);

    //Test quand on veut déplacer sur de l'eau
    get_move_message(MOVE_WATER, buf, sizeof(buf));
    assert(strcmp(buf, "Impossible : eau infranchissable") == 0);

    //Test quand on veut déplacer une unité hors de sa portée
    get_move_message(MOVE_NOT_ADJACENT, buf, sizeof(buf));
    assert(strcmp(buf, "Il faut se deplacer case par case") == 0);

    //Test quand on veut déplacer une unité sur une tuile qui requiert + de PM qu'elle n'en a (genre la montagne)
    get_move_message(MOVE_NOT_ENOUGH_PM, buf, sizeof(buf));
    assert(strcmp(buf, "Pas assez de PM") == 0);

    //Test quand on veut mettre une unité sur une tuile avec une autre unité à nous
    get_move_message(MOVE_ALLY_OCCUPIED, buf, sizeof(buf));
    assert(strcmp(buf, "Case occupee par une unite alliee") == 0);
}

int main(void) {
    printf("Démarrage des tests liés à l'affichage des déplacements des unités dans sdl_panneaux...\n");
    test_get_move_message_all_cases();
    printf("=> TOUS LES CAS DE MOUVEMENT ONT REUSSI ! (8/8 assertions OK)\n");
    return 0;
}
