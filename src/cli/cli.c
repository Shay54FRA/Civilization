#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "../map/map.h"
#include "../game/game.h"
#include "../tile/tile.h"

// Gestion des unités et des technologies dans le CLI
#include "../unit/unit.h"
#include "../unit/unit_cli.h"
#include "../technology/technology.h"
#include "../technology/technology_cli.h"

// Le champ de vision de la carte
#define VIEW_RADIUS 6

// Taille maximale du message affiché dans le HUD
#define MSG_SIZE 256


void print_pos(Position pos) {
    printf("Position : (%d, %d)", pos.x, pos.y);
}


// Renvoie le nom lisible d’un biome
static const char* biome_name(char biome) {
    switch (biome) {
        case 'E': return "Eau";
        case 'P': return "Plaine";
        case 'F': return "Foret";
        case 'M': return "Montagne";
        case 'D': return "Desert";
        case 'T': return "Toundra";
        default: return "Inconnu";
    }
}


// Renvoie le coût de déplacement selon le terrain
static int terrain_move_cost(char biome) {
    if (biome == 'E') return -1;
    if (biome == 'F' || biome == 'M') return 2;
    return 1;
}

// Affiche les informations de la case sélectionnée
static void print_tile_info(Game* game, Position cursor) {
    Tile* tile = get_tile(game->map, cursor);

    if (!tile) return;

    int cost = terrain_move_cost(tile->biome);

    printf("\n=== CASE SELECTIONNEE ===\n");

    printf("Position : (%d, %d)\n", cursor.x, cursor.y);
    printf("Terrain  : %s\n", biome_name(tile->biome));

    if (cost == -1)
        printf("Cout PM  : Infranchissable\n");
    else
        printf("Cout PM  : %d\n", cost);

    if (tile->city_on)
        printf("Contenu  : Ville\n");

    else if (tile->unit)
        printf("Contenu  : Unite %s [%c]\n", get_name(tile->unit->type), tile->unit->type);

    else
        printf("Contenu  : Vide\n");
}


// Affiche les statistiques de l’unité sélectionnée
static void print_selected_unit_info(Unit* selected_unit) {
    printf("\n=== UNITE SELECTIONNEE ===\n");

    if (!selected_unit) {
        printf("Aucune unite selectionnee.\n");
        return;
    }

    printf("Type : %s [%c]\n", get_name(selected_unit->type), selected_unit->type);

    printf("PV   : %d / %d\n", selected_unit->pv, selected_unit->max_pv);

    printf("PM   : %d / %d\n", selected_unit->pm, selected_unit->max_pm);

    printf("ATK  : %d\n", selected_unit->atk);
    printf("DEF  : %d\n", selected_unit->def);

    printf("Pos  : (%d, %d)\n", selected_unit->pos.x, selected_unit->pos.y);
}


// Convertit le résultat d’un déplacement en message joueur
static void move_result_to_message(MoveResult result, char* buffer, size_t size) {
    switch (result) {

        case MOVE_OK:
            snprintf(buffer, size, "Deplacement effectue !"); break;

        case MOVE_NO_UNIT:
            snprintf(buffer, size, "Aucune unite selectionnee."); break;

        case MOVE_NO_PM:
            snprintf(buffer, size, "Cette unite n'a plus de PM."); break;

        case MOVE_INVALID_TILE:
            snprintf(buffer, size, "Case invalide."); break;

        case MOVE_WATER:
            snprintf(buffer, size, "Impossible : eau infranchissable."); break;

        case MOVE_NOT_ADJACENT:
            snprintf(buffer, size, "Il faut se deplacer case par case."); break;

        case MOVE_NOT_ENOUGH_PM:
            snprintf(buffer, size, "Pas assez de PM."); break;

        case MOVE_ALLY_OCCUPIED:
            snprintf(buffer, size, "Case occupee par une unite."); break;

        default:
            snprintf(buffer, size, "Action impossible."); break;
    }
}


void run_game_cli(Game* game) {
    int running = 1;
    char command;
    Position cursor = {0, 0}; // Position initiale de la caméra

    // Unité actuellement sélectionnée
    Unit* selected_unit = NULL;

    // Message affiché dans le HUD
    char last_message[MSG_SIZE] = "Bienvenue dans Civ PP2ix.";

    while (running) {

        // Affichage
        system("clear"); //permet de clear le terminal
        print_map_cli(game->map, cursor);
        
        // Menu des stats
        printf("\n--- TOUR %d | Or: %d | Science: %d ---\n", 
                game->active_turn, game->gold, game->science);

        // Affichage du dernier message d’action
        printf("Message : %s\n", last_message);

        // Affichage des informations de la case et de l’unité sélectionnée
        print_tile_info(game, cursor);
        print_selected_unit_info(selected_unit);

        printf("\nCommandes : [z/q/s/d] Déplacer caméra | [m] Sélectionner/Déplacer unité | [v] Fonder ville | [t] Technologies | [f] Fin de tour | [x] Quitter\n");
        printf("> ");
        
        // Récupération de l'ordre
        scanf(" %c", &command); // L'espace avant %c ignore les retours à la ligne

        // Logique de commande
        switch (command) {

            case 'z':
                if (cursor.y > 0) cursor.y--; break;

            case 's':
                if (cursor.y < game->map->height - 1) cursor.y++; break;

            case 'q':
                if (cursor.x > 0) cursor.x--; break;

            case 'd':
                if (cursor.x < game->map->length - 1) cursor.x++; break;

            // Sélection et déplacement des unités
            case 'm':
                if (selected_unit == NULL) {
                    Tile* tile = get_tile(game->map, cursor);

                    if (tile && tile->unit) {
                        selected_unit = tile->unit;
                        snprintf(last_message, MSG_SIZE, "Unite %s [%c] selectionnee.", get_name(selected_unit->type), selected_unit->type);
                    } else {
                        snprintf(last_message, MSG_SIZE, "Aucune unite sur cette case.");
                    }
                }
                else {
                    MoveResult result = move_unit_step(game, selected_unit, cursor);
                    move_result_to_message(result, last_message, MSG_SIZE);
                    selected_unit = NULL;
                }
                break;

            // Fondation d'une ville par un colon
            case 'v':
                if (selected_unit == NULL) {
                    snprintf(last_message, MSG_SIZE, "Aucune unite selectionnee.");
                }
                else if (selected_unit->type != 'c') {
                    snprintf(last_message, MSG_SIZE, "Seul un Colon peut fonder une ville.");
                }
                else {
                    Position city_pos = selected_unit->pos;
                    Tile* tile = get_tile(game->map, city_pos);

                    if (tile != NULL && tile->city_on) {
                        snprintf(last_message, MSG_SIZE, "Impossible : il y a deja une ville ici.");
                    }

                    else {
                        colonize(game, selected_unit);
                        selected_unit = NULL;

                        snprintf(last_message, MSG_SIZE, "Ville fondee en (%d, %d).", city_pos.x, city_pos.y);
                    }
                }
                break;

            // Ouverture du menu des technologies
            case 't':
                system("clear");
                show_technology_menu(game);
                snprintf(last_message, MSG_SIZE, "Retour arbre technologique.");
                break;

            case 'f':
                printf("Passage au tour suivant...\n");

                // Passage au tour suivant
                game->active_turn++;

                snprintf(last_message, MSG_SIZE, "Tour suivant.");
                break;
                
            case 'x':
                running = 0; break;

            default:
                snprintf(last_message, MSG_SIZE, "Commande inconnue.");
                break;
        }
    }
    printf("Retour au menu principal...\n");
}
