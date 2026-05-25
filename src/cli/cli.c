#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "cli.h"
#include "../map/map.h"
#include "../game/game.h"
#include "../tile/tile.h"
#include "../barbarian/barbarian.h"
#include "../building/building.h"
<<<<<<< HEAD
#include "cli_panneaux.h"
=======
#include "../city/city.h"
>>>>>>> 5093143 (menu des projets)

// Gestion des unités et des technologies dans le CLI
#include "../unit/unit.h"
#include "../unit/unit_cli.h"
#include "../technology/technology.h"
#include "../technology/technology_cli.h"

WINDOW *win_map, *win_info, *win_hud; //Permet de créer plusieurs fenêtres ncurses en même temps
WINDOW *in_map, *in_info, *in_hud; //Je crée des sous-fenêtres pour écrire dedans

void setup_windows(void) { //Je crée cette fonction pour refresh l'affichage des fenêtres CLI si elles sont bug et qu'on change la taille du terminal avec ./civ
    // 1. Détruire les anciennes fenêtres si elles existent (pour libérer la mémoire)
    if (in_map != NULL) delwin(in_map);
    if (in_hud != NULL) delwin(in_hud);
    if (in_info != NULL) delwin(in_info);
    if (win_map != NULL) delwin(win_map);
    if (win_hud != NULL) delwin(win_hud);
    if (win_info != NULL) delwin(win_info);

    // 2. Récupérer les NOUVELLES dimensions du terminal
    int h = LINES, w = COLS;
    int footer_height = 11; //correspond à la hauteur du bas. Plus ce nb est grand, plus win_info est haute 
    int top_height = h - footer_height;

    // 3. Recréer les fenêtres aux bonnes dimensions
    win_map = newwin(top_height, (w * 7) / 10, 0, 0);
    win_hud = newwin(top_height, (w * 3) / 10, 0, (w * 7) / 10);
    win_info = newwin(footer_height, w, top_height, 0);
    
    // 4. Créer les sous-fenêtres de CONTENU (décalées de 1 en X et Y pour éviter d'écrire sur les bordures)
    // derwin(parent, hauteur, largeur, position_y, position_x)
    in_map = derwin(win_map, top_height - 2, ((w * 7) / 10) - 2, 1, 1);
    in_hud = derwin(win_hud, top_height - 2, ((w * 3) / 10) - 2, 1, 1);
    in_info = derwin(win_info, footer_height - 2, w - 2, 1, 1);

}

void init_ncurses_interface(void) {
    initscr(); //lance l'affichage ncurses           
    cbreak();  // rend l'affichage interactif. Quand on tape "d", ça nous déplace direct vers la droite par exemple en utilisant getch() au lieu de scanf()                              
    keypad(stdscr, TRUE); //Permet de détecter le changement de taille du terminal ainsi que d'ajouter des touches comme les flèches du claiver par exemple.
    
    // Vérifier que le terminal est assez grand avant de lancer l'affichage CLI pour éviter trop de bugs graphiques
    if (LINES < 30 || COLS < 80) {
        endwin(); // On ferme proprement ncurses
        printf("Erreur : Terminal trop petit ! Veuillez agrandir la fenêtre \n");
        exit(1); 
    }

    setup_windows();

    if (has_colors()) {
        start_color();
        use_default_colors();

        init_pair(COLOR_EAU, COLOR_BLACK, COLOR_BLUE); //Les couleurs de bg et fg sont forcément définies par pairs avec ncurses
        init_pair(COLOR_PLAINE, COLOR_BLACK, COLOR_GREEN);
        init_pair(COLOR_FORET, COLOR_WHITE, COLOR_GREEN);
        init_pair(COLOR_MONTAGNE, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOR_DESERT, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COLOR_TOUNDRA, COLOR_BLACK, COLOR_CYAN);
        init_pair(COLOR_VILLE, COLOR_WHITE, COLOR_MAGENTA);
        init_pair(COLOR_CURSEUR, COLOR_RED, -1); // -1 = transparent
    }
}

// Taille maximale du message affiché dans le HUD
#define MSG_SIZE 256


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

void end_game_cli(WINDOW* win,Game* game, int end_code) {
    if (game == NULL) return;
    wprintw(win,"\n=== FIN DE LA PARTIE ===\n\n");
    if (end_code == 1) {
        wprintw(win,"----VICTOIRE TERRITORIALE ! \nVous possédez plus de 10 villes depuis 5 tours.\n");
    }
    else if (end_code == 2) {
        wprintw(win,"----VICTOIRE TECHNOLOGIQUE ! \nVous possédez toutes les technologies.\n");
    }
    else if (end_code == 3) {
        wprintw(win,"----DEFAITE !\n");
    }
    else {
        wprintw(win,"ERREUR !\n");
    }
    wprintw(win,"\nSCORE : %d\n", game_score(game));
    wprintw(win,"\n\n\n cliquez sur n'importe quelle touche pour terminer");
    getch();
}

void run_game_cli(Game* game) {
    init_ncurses_interface(); // DÉMARRAGE DE NCURSES


    int running = 1;
    int command; //Je remplace le char par un int car getch() renvoie un entier pour les touches spéciales comme KEY_RESIZE
    Position cursor = {0, 0}; // Position initiale de la caméra

    // Unité actuellement sélectionnée
    Unit* selected_unit = NULL;

    // Message affiché dans le HUD
    char last_message[MSG_SIZE] = "Bienvenue dans Civ PP2ix.";

    while (running) {

        //BOXE 1 : Affichage map
        werase(win_map);
        box(win_map, 0, 0);
        mvwprintw(win_map, 0, 2, " CARTE ");
        wmove(in_map, 0, 0); //Réinitialise position curseur
        print_map_cli(in_map,game->map, cursor);
        touchwin(win_map); //Juste par sécurité
        wrefresh(win_map);
        
        //BOXE 2 : Affichage des infos générales
        werase(win_info);
        box(win_info, 0, 0);
        mvwprintw(win_info, 0, 2, " Infos générales ");
        wmove(in_info, 0, 0); //Réinitialise position curseur
        print_stats(in_info,game); // Affichage des stats ()
        wprintw(in_info,"Message : %s\n", last_message); // Affichage du dernier message d’action
        print_action_help(in_info); // Affichage des commandes
        touchwin(win_info); //Juste par sécurité
        wrefresh(win_info);

        //BOXE 3 : Affichage info case/unité sélectionné
        werase(win_hud);
        box(win_hud, 0, 0);
        mvwprintw(win_hud, 0, 2, " Infos case ");
        wmove(in_hud, 0, 0); //Réinitialise position curseur
        print_tile_info(in_hud,game, cursor);
        print_selected_unit_info(in_hud,selected_unit);
        Tile* tile_sous_curseur = get_tile(game->map, cursor);
        if (tile_sous_curseur != NULL && tile_sous_curseur->city_on) {
            show_city_info_cli(in_hud,game,cursor); //Affichage automatique des infos de la ville
        }
        touchwin(win_hud); //Juste par sécurité
        wrefresh(win_hud);
        

        command = getch(); // Remplaçant de scanf, lit la touche instantanément

        // Logique de commande
        switch (command) {
            case KEY_RESIZE: //touche spéciale qui détecte si on change la taille de notre terminal. Et si c'est le cas, on redimensionne automatiquement les fenêtres CLI avec setup_windows()
                clear();
                refresh();
                setup_windows();
                snprintf(last_message,MSG_SIZE,"Fen^tre redimensionnée !");
                break;
            case 'z': if (cursor.y > 0) cursor.y--; break;
            case 's': if (cursor.y < game->map->height - 1) cursor.y++; break;
            case 'q': if (cursor.x > 0) cursor.x--; break;
            case 'd': if (cursor.x < game->map->length - 1) cursor.x++; break;

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
                clear(); //clear le terminal
                show_technology_menu(game);
                snprintf(last_message, MSG_SIZE, "Retour arbre technologique.");
                break;

            case 'f':
                wprintw(win_info,"Passage au tour suivant...\n");
                int game_result = end_turn(game);
                if (game_result != 0) {
                    clear();
                    end_game_cli(win_info,game, game_result);
                    running = 0;
                    break;
                }
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

    endwin(); // CRUCIAL : Rend le terminal normal
    printf("Retour au menu principal...\n"); // Celui-ci reste en printf car on a quitté ncurses
}
