#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "../map/map.h"
#include "../game/game.h"
#include "../tile/tile.h"



void print_pos(Position pos) {
    printf("Position : (%d, %d)", pos.x, pos.y);
}



void run_game_cli(Game* game) {
    int running = 1;
    char command;
    Position cursor = {0, 0}; // Position initiale de la caméra

    while (running) {
        // Affichage
        system("clear"); //permet de clear le terminal
        print_map_cli(game->map, cursor);
        
        // Menu d'interaction
        printf("\n--- TOUR %d | Or: %d | Science: %d ---\n", 
                game->active_turn, game->gold, game->science);
        printf("Commandes : [z/q/s/d] Déplacer caméra | [f] Fin de tour | [x] Quitter\n");
        printf("> ");
        
        // Récupération de l'ordre
        scanf(" %c", &command); // L'espace avant %c ignore les retours à la ligne

        // Logique de commande
        switch (command) {
            case 'z': if (cursor.y > 0) cursor.y--; break;
            case 's': if (cursor.y < game->map->height - 1) cursor.y++; break;
            case 'q': if (cursor.x > 0) cursor.x--; break;
            case 'd': if (cursor.x < game->map->length - 1) cursor.x++; break;
            case 'f': 
                printf("Passage au tour suivant...\n");
                // Appeler ici ta fonction de fin de tour (calcul ressources, etc.)
                game->active_turn++;
                break;
            case 'x':
                running = 0;
                break;
            default:
                printf("Commande inconnue !\n");
                break;
        }
    }
    printf("Retour au menu principal...\n");
}
