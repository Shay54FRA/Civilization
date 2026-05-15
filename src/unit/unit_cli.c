#include <stdio.h>
#include "unit_cli.h"

void print_move_result_cli(MoveResult result)
{
    switch (result) {
        case MOVE_OK:
            printf("\n-> Deplacement effectue !\n");
            break;

        case MOVE_NO_UNIT:
            printf("\n-> Aucune unite selectionnee.\n");
            break;

        case MOVE_NO_PM:
            printf("\n-> Cette unite n'a plus de PM.\n");
            break;

        case MOVE_INVALID_TILE:
            printf("\n-> Case invalide.\n");
            break;

        case MOVE_WATER:
            printf("\n-> Deplacement impossible : eau infranchissable.\n");
            break;

        case MOVE_NOT_ADJACENT:
            printf("\n-> Deplacement impossible : il faut bouger case par case.\n");
            break;

        case MOVE_NOT_ENOUGH_PM:
            printf("\n-> Deplacement impossible : pas assez de PM.\n");
            break;

        case MOVE_ALLY_OCCUPIED:
            printf("\n-> Case deja occupee par une unite alliee.\n");
            break;

        case MOVE_ATTACK_FORBIDDEN:
            printf("\n-> Cette unite ne peut pas attaquer.\n");
            break;

        case MOVE_COMBAT_WIN:
            printf("\n-> Combat gagne, l'ennemi est detruit.\n");
            break;

        case MOVE_COMBAT_BLOCKED:
            printf("\n-> Combat resolu, mais la cible est toujours presente.\n");
            break;

        case MOVE_ATTACKER_DEAD:
            printf("\n-> Votre unite a ete detruite en attaquant.\n");
            break;

        default:
            printf("\n-> Action impossible.\n");
            break;
    }
}
