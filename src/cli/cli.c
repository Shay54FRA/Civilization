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


// Renvoie le nom lisible d’une unité
static const char* unit_name(char type) {
    switch (type) {
        case 'c': return "Colon";
        case 'g': return "Guerrier";
        default: return "Inconnue";
    }
}


// Calcule le coût total d'entretien des unités
static int total_unit_maintenance(UnitList* unitList) {
    int total = 0;
    UnitList* current = unitList;

    while (current != NULL) {
        if (current->data != NULL) {
            total += current->data->cost_per_turn;
        }

        current = current->next;
    }

    return total;
}


void print_map_cli(Map* m, Position cursor) {
    if (m == NULL || m->map == NULL) return;
    system("clear"); //permet de clear le terminal

    int start_y = cursor.y - VIEW_RADIUS;
    int end_y = cursor.y + VIEW_RADIUS;
    int start_x = cursor.x - VIEW_RADIUS;
    int end_x = cursor.x + VIEW_RADIUS;

    for (int y = start_y; y <= end_y; y++) {
        
        // Pour faire de grosses cases, on dessine sur 3 lignes
        for (int line = 0; line < 3; line++) {
            
            // Pour faire un effet hexagone, on décale les lignes impaires
            if (y % 2 != 0) {
                printf("     "); // On décale de 5 espaces car nos cases font 9 de large + 1 espace de séparation
            }

            for (int x = start_x; x <= end_x; x++) {
                
                // Si la caméra regarde dans le vide (hors carte)
                if (x < 0 || x >= m->length || y < 0 || y >= m->height) {
                    printf("     "); // 5 espaces
                    continue;
                }

                Tile* tuile = m->map[y][x];
                
                // 1. Choix du symbole à afficher sur la case
                char symbol = ' ';
                if (tuile->city_on) symbol = 'V';
                else if (tuile->unit) symbol = 'U';

                // 2. Gestion des couleurs des cases

                const char* bg = "";
                const char* fg = "\x1b[30m";
                
                if (tuile->city_on){
                    bg = COLOR_VILLE;
                    fg = "\x1b[31;1m";
                }
                else if (tuile->unit){
                    bg = COLOR_UNITE;
                    fg = "\x1b[31;1m";
                }
                else {
                    switch(tuile->biome) {
                        case 'E': bg = BG_EAU; break;
                        case 'P': bg = BG_PLAINE; break;
                        case 'F': bg = BG_FORET; break;
                        case 'M': bg = BG_MONTAGNE; break;
                        case 'D': bg = BG_DESERT; break;
                        case 'T': bg = BG_TOUNDRA; break;
                    }
                }

                // 3. DESSIN D'UNE CASE

                if (line == 0 || line == 2) {

                    if (cursor.x == x && cursor.y == y) {
                        printf("%s\x1b[31;1m+-------+%s ", bg, COLOR_RESET);
                    }

                    else {
                        printf("%s         %s ", bg, COLOR_RESET);
                    }
                }
                
                else if (line == 1) {

                    if (cursor.x == x && cursor.y == y) { 
                        printf("%s\x1b[31;1m|%s   %c   \x1b[31;1m|%s ",
                               bg, fg, symbol, COLOR_RESET);
                    }

                    else {
                        printf("%s%s    %c    %s ",
                               bg, fg, symbol, COLOR_RESET);
                    }
                }
            }

            printf("\n");
        }

        printf("\n");
    }
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
        printf("Contenu  : Unite %s [%c]\n",
               unit_name(tile->unit->type),
               tile->unit->type);

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

    printf("Type : %s [%c]\n",
           unit_name(selected_unit->type),
           selected_unit->type);

    printf("PV   : %d / %d\n",
           selected_unit->pv,
           selected_unit->max_pv);

    printf("PM   : %d / %d\n",
           selected_unit->pm,
           selected_unit->max_pm);

    printf("ATK  : %d\n", selected_unit->atk);
    printf("DEF  : %d\n", selected_unit->def);

    printf("Pos  : (%d, %d)\n",
           selected_unit->pos.x,
           selected_unit->pos.y);
}


// Convertit le résultat d’un déplacement en message joueur
static void move_result_to_message(MoveResult result,
                                   char* buffer,
                                   size_t size) {
    switch (result) {

        case MOVE_OK:
            snprintf(buffer, size,
                     "Deplacement effectue !");
            break;

        case MOVE_NO_UNIT:
            snprintf(buffer, size,
                     "Aucune unite selectionnee.");
            break;

        case MOVE_NO_PM:
            snprintf(buffer, size,
                     "Cette unite n'a plus de PM.");
            break;

        case MOVE_INVALID_TILE:
            snprintf(buffer, size,
                     "Case invalide.");
            break;

        case MOVE_WATER:
            snprintf(buffer, size,
                     "Impossible : eau infranchissable.");
            break;

        case MOVE_NOT_ADJACENT:
            snprintf(buffer, size,
                     "Il faut se deplacer case par case.");
            break;

        case MOVE_NOT_ENOUGH_PM:
            snprintf(buffer, size,
                     "Pas assez de PM.");
            break;

        case MOVE_ALLY_OCCUPIED:
            snprintf(buffer, size,
                     "Case occupee par une unite.");
            break;

        default:
            snprintf(buffer, size,
                     "Action impossible.");
            break;
    }
}


void run_game_cli(Game* game) {
    int running = 1;
    char command;
    Position cursor = {0, 0}; // Position initiale de la caméra

    // Unité actuellement sélectionnée
    Unit* selected_unit = NULL;

    // Message affiché dans le HUD
    char last_message[MSG_SIZE] =
        "Bienvenue dans Civ PP2ix.";

    while (running) {

        // Affichage
        print_map_cli(game->map, cursor);
        
        // Menu d'interaction
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
        scanf(" %c", &command);

        // Logique de commande
        switch (command) {

            case 'z':
                if (cursor.y > 0) cursor.y--;
                break;

            case 's':
                if (cursor.y < game->map->height - 1) cursor.y++;
                break;

            case 'q':
                if (cursor.x > 0) cursor.x--;
                break;

            case 'd':
                if (cursor.x < game->map->length - 1) cursor.x++;
                break;


            // Sélection et déplacement des unités
            case 'm':

                if (selected_unit == NULL) {

                    Tile* tile =
                        get_tile(game->map, cursor);

                    if (tile && tile->unit) {

                        selected_unit = tile->unit;

                        snprintf(last_message,
                                 MSG_SIZE,
                                 "Unite %s [%c] selectionnee.",
                                 unit_name(selected_unit->type),
                                 selected_unit->type);
                    }

                    else {

                        snprintf(last_message,
                                 MSG_SIZE,
                                 "Aucune unite sur cette case.");
                    }
                }

                else {

                    MoveResult result =
                        move_unit_step(game,
                                       selected_unit,
                                       cursor);

                    move_result_to_message(result,
                                           last_message,
                                           MSG_SIZE);

                    selected_unit = NULL;
                }

                break;


            // Fondation d'une ville par un colon
            case 'v':

                if (selected_unit == NULL) {

                    snprintf(last_message,
                             MSG_SIZE,
                             "Aucune unite selectionnee.");
                }

                else if (selected_unit->type != 'c') {

                    snprintf(last_message,
                             MSG_SIZE,
                             "Seul un Colon peut fonder une ville.");
                }

                else {

                    Position city_pos = selected_unit->pos;
                    Tile* tile = get_tile(game->map, city_pos);

                    if (tile != NULL && tile->city_on) {

                        snprintf(last_message,
                                 MSG_SIZE,
                                 "Impossible : il y a deja une ville ici.");
                    }

                    else {

                        colonize(game, selected_unit);
                        selected_unit = NULL;

                        snprintf(last_message,
                                 MSG_SIZE,
                                 "Ville fondee en (%d, %d).",
                                 city_pos.x,
                                 city_pos.y);
                    }
                }

                break;


            // Ouverture du menu des technologies
            case 't':

                system("clear");

                show_technology_menu(game);

                snprintf(last_message,
                         MSG_SIZE,
                         "Retour arbre technologique.");

                break;


            case 'f':

                printf("Passage au tour suivant...\n");

                game->active_turn++;

                // Mise à jour des ressources et des projets de ville
                give_all_bonuses(game);
                update_city_projects(game);

                // Mise à jour des recherches et des points de mouvement
                int tech_id =
                    update_research(game);

                int maintenance =
                    total_unit_maintenance(game->unitList);

                game->gold -= maintenance;

                if (tech_id != -1) {

                    snprintf(last_message,
                             MSG_SIZE,
                             "Technologie debloquee : %s. Entretien unites : -%d or.",
                             game->tech_tree
                                 ->technologies[tech_id]
                                 .name,
                             maintenance);
                }

                else {

                    snprintf(last_message,
                             MSG_SIZE,
                             "Tour suivant. Entretien unites : -%d or.",
                             maintenance);
                }

                reset_all_pm(game->unitList);

                break;

            case 'x':
                running = 0;
                break;

            default:

                snprintf(last_message,
                         MSG_SIZE,
                         "Commande inconnue.");

                break;
        }
    }

    printf("Retour au menu principal...\n");
}
