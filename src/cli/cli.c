#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <locale.h>

#include "cli.h"
#include "../map/map.h"
#include "../game/game.h"
#include "../tile/tile.h"
#include "../barbarian/barbarian.h"
#include "../building/building.h"
#include "cli_panneaux.h"
#include "../city/city.h"

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
    int footer_height = 13; //correspond à la hauteur du bas. Plus ce nb est grand, plus win_info est haute 
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
        init_pair(COLOR_FORET, COLOR_WHITE, 22);
        init_pair(COLOR_MONTAGNE, COLOR_BLACK, COLOR_WHITE);
        init_pair(COLOR_DESERT, COLOR_BLACK, COLOR_YELLOW);
        init_pair(COLOR_TOUNDRA, COLOR_BLACK, COLOR_CYAN);
        init_pair(COLOR_VILLE, COLOR_WHITE, COLOR_MAGENTA);
        init_pair(COLOR_BROUILLARD, COLOR_WHITE, 8);
        init_pair(COLOR_CURSEUR, COLOR_RED, -1); // -1 = transparent
        init_pair(COLOR_ROUGE, COLOR_RED, -1);
        init_pair(COLOR_JAUNE, COLOR_YELLOW, -1);
        init_pair(COLOR_VERT, COLOR_GREEN, -1);
    }
}

// Taille maximale du message affiché dans le HUD
#define MSG_SIZE 256

#define MAX_HISTORY 50 //

void add_to_history(char history[MAX_HISTORY][MSG_SIZE], int* count, const char* msg) {
    if (msg == NULL || msg[0] == '\0') return;

    if (*count < MAX_HISTORY) {
        strncpy(history[*count], msg, MSG_SIZE);
        history[*count][MSG_SIZE - 1] = '\0';
        (*count)++;
    } else {
        // Si le tableau est plein, on décale tout vers le haut (on perd le plus vieux)
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strncpy(history[i], history[i+1], MSG_SIZE);
        }
        strncpy(history[MAX_HISTORY - 1], msg, MSG_SIZE);
        history[MAX_HISTORY - 1][MSG_SIZE - 1] = '\0';
    }
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

void show_city_project_menu(Game* game, City* city,char* message) {
    if (city == NULL) return;
    if (city->project != NULL) return;
    char choix;
    while (1) {
        printw("\n===== CHOIX DU PROJET =====\n");
        if (city->can_produce_unit)  {
            printw("\n---- UNIT ----\n\n");
            printw("-> %s [c] - %d production - %d or/tour [ 10pv - 0atk - 1déf - 2pm]\n", get_name('c'), get_cost('c'), get_entretien_cost('c'));
            if (is_unit_unlocked(game->tech_tree, 'g')) {
                printw("-> %s [g] - %d production - %d or/tour [ 15pv - 3atk - 2déf - 3pm]\n", get_name('g'), get_cost('g'), get_entretien_cost('g'));
            }
            if (is_unit_unlocked(game->tech_tree, 'e')) {
                printw("-> %s [e] - %d production - %d or/tour [ 10pv - 0atk - 2déf - 4pm - 4 vision range]\n", get_name('e'), get_cost('e'), get_entretien_cost('e'));
            }
        }
        printw("\n---- BUILDING ----\n\n"); // MODIFICATION DE L'AFFICHAGE (On affiche 1, 2, 3, b, m, r mais on garde G, A, C... en interne)
        printw("-> %s [G] - %d production - %d or/tour : +3 food/tour et coût en nourriture de croissance/1.5\n", get_name('G'), get_cost('G'), get_entretien_cost('G'));
        printw("-> %s [a] - %d production - %d or/tour : +3 prod/tour\n", get_name('A'), get_cost('A'), get_entretien_cost('A'));
        printw("-> %s [C] - %d production - %d or/tour : Débloque la possibilité de fonder des unités\n", get_name('C'), get_cost('C'), get_entretien_cost('C'));
        if (is_building_unlocked(game->tech_tree, 'B')) {
            printw("-> %s [b] - %d production - %d or/tour : +4 science/tour\n", get_name('B'), get_cost('B'), get_entretien_cost('B'));
        }
        if (is_building_unlocked(game->tech_tree, 'M')) {
            printw("-> %s [m] - %d production - %d or/tour : +3 gold/tour\n", get_name('M'), get_cost('M'), get_entretien_cost('M'));
        }
        if (is_building_unlocked(game->tech_tree, 'R')) {
            printw("-> %s [r] - %d production - %d or/tour : pv x2 et force de la ville +10\n", get_name('R'), get_cost('R'), get_entretien_cost('R'));
        }
        if (is_building_unlocked(game->tech_tree, 'P')) {
            printw("-> %s [p] - %d production - %d or/tour : +2 rayon de vision\n", get_name('P'), get_cost('P'), get_entretien_cost('P'));
        }

        printw("\n\n\n\n\nPour commencer un projet entrez n'importe lequel des boutons encadrés [..], et n'importe quel autre pour quitter : ");
        choix = getch();
        if (city->can_produce_unit) {
            if (choix == 'c') {
                start_project(city, choix, city->pos);
            }
            if (choix == 'g' && is_unit_unlocked(game->tech_tree, 'g')) {
                start_project(city, choix, city->pos);
            }
            if (choix == 'e' && is_unit_unlocked(game->tech_tree, 'e')) {
                start_project(city, choix, city->pos);
            }
        }
        if (choix == 'm' && is_building_unlocked(game->tech_tree, 'M')) start_project(city, choix, city->pos);
        else if (choix == 'b' && is_building_unlocked(game->tech_tree, 'B')) start_project(city, choix, city->pos);
        else if (choix == 'r' && is_building_unlocked(game->tech_tree, 'R')) start_project(city, choix, city->pos);
        else if (choix == 'p' && is_building_unlocked(game->tech_tree, 'P')) start_project(city, choix, city->pos);
        else if (choix == 'G') start_project(city, 'G', city->pos);
        else if (choix == 'a') start_project(city, 'A', city->pos);
        else if (choix == 'C') start_project(city, 'C', city->pos);

        // Message de confirmation
        if (city->project != NULL) {
            snprintf(message, 256, "Vous avez lance la construction : %s", get_name(city->project->type));
        } else {
            snprintf(message, 256, "Menu ville quitte (Aucun projet lance).");
        }

        return;
    }
}

void show_history_menu(char history[MAX_HISTORY][MSG_SIZE], int count) {
    clear();
    
    printw("\n===== HISTORIQUE DES EVENEMENTS =====\n\n");
    
    if (count == 0) {
        printw("Aucun evenement pour le moment.\n");
    } else {
        // On calcule le point de départ pour ne pas afficher plus de messages 
        // qu'il n'y a de lignes dans le terminal (LINES - 5 pour la marge)
        int start = 0;
        if (count > LINES - 5) {
            start = count - (LINES - 5); 
        }
        
        // On affiche les messages du plus ancien au plus récent
        for (int i = start; i < count; i++) {
            printw(" > %s\n", history[i]);
        }
    }
    
    printw("\n\n[ Appuyez sur n'importe quelle touche pour fermer ]");
    
    refresh();
    getch();
}

void run_game_cli(Game* game) {
    init_ncurses_interface(); // DÉMARRAGE DE NCURSES


    int running = 1;
    int command; //Je remplace le char par un int car getch() renvoie un entier pour les touches spéciales comme KEY_RESIZE
    Position cursor = get_starting_city_pos(game->map); // Position initiale de la caméra

    // Unité actuellement sélectionnée
    Unit* selected_unit = NULL;

    // Message affiché dans le HUD
    char last_message[MSG_SIZE] = "Bienvenue dans Civ PP2ix.";

    // Création d'une historique
    char history[MAX_HISTORY][MSG_SIZE];
    int history_count = 0;
    add_to_history(history, &history_count, last_message); //On ajoute le 1er msg

    while (running) {

        //BOXE 1 : Affichage map
        werase(win_map);
        box(win_map, 0, 0);
        mvwprintw(win_map, 0, 2, " CARTE ");
        wmove(in_map, 0, 0); //Réinitialise position curseur
        update_fog(game);
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
            wrefresh(win_hud);
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
                snprintf(last_message,MSG_SIZE,"Fenetre redimensionnée !");
                add_to_history(history, &history_count, last_message);
                
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
                        add_to_history(history, &history_count, last_message);
                    } else {
                        snprintf(last_message, MSG_SIZE, "Aucune unite sur cette case.");
                        add_to_history(history, &history_count, last_message);
                    }
                }
                else {
                    MoveResult result = move_unit_step(game, selected_unit, cursor);
                    move_result_to_message(result, last_message, MSG_SIZE);
                    add_to_history(history, &history_count, last_message);
                    selected_unit = NULL;
                }
                break;

            // Fondation d'une ville par un colon
            case 'v':
                if (selected_unit == NULL) {
                    snprintf(last_message, MSG_SIZE, "Aucune unite selectionnee.");
                    add_to_history(history, &history_count, last_message);
                }
                else if (selected_unit->type != 'c') {
                    snprintf(last_message, MSG_SIZE, "Seul un Colon peut fonder une ville.");
                    add_to_history(history, &history_count, last_message);
                }
                else {
                    Position city_pos = selected_unit->pos;
                    Tile* tile = get_tile(game->map, city_pos);

                    if (tile != NULL && tile->city_on) {
                        snprintf(last_message, MSG_SIZE, "Impossible : il y a deja une ville ici.");
                        add_to_history(history, &history_count, last_message);
                    }

                    else {
                        colonize(game, selected_unit);
                        selected_unit = NULL;

                        snprintf(last_message, MSG_SIZE, "Ville fondee en (%d, %d).", city_pos.x, city_pos.y);
                        add_to_history(history, &history_count, last_message);
                    }
                }
                break;

            // Ouverture du menu des technologies
            case 't':
                clear(); //clear le terminal
                show_technology_menu(game);
                snprintf(last_message, MSG_SIZE, "Retour arbre technologique.");
                add_to_history(history, &history_count, last_message);
                break;

            case 'r':{
                Tile* tile = get_tile(game->map, cursor);
                if (!tile->city_on) {
                    snprintf(last_message, MSG_SIZE, "Tu n'es pas sur une ville !");
                    add_to_history(history, &history_count, last_message);
                    break;
                } else {
                    City* city = get_city_on_tile(game->cityList, tile);
                    if (city->project != NULL) {
                        snprintf(last_message, MSG_SIZE, "Un projet est déjà lancé !");
                        add_to_history(history, &history_count, last_message);
                        break;
                    } else {
                        clear();
                        show_city_project_menu(game, city,last_message);
                        snprintf(last_message, MSG_SIZE, "Retour menu ville");
                        add_to_history(history, &history_count, last_message);
                        break;
                    }
                }
            }

            case 'f':{
                wprintw(win_info,"Passage au tour suivant...\n");
                int game_result = end_turn(game);
                if (game_result != 0) {
                    end_game_cli(win_info,game, game_result);
                    running = 0;
                    break;
                }
                game->active_turn++;
                snprintf(last_message, MSG_SIZE, "Tour suivant.");
                add_to_history(history, &history_count, last_message);
                break;
            }
                
            case 'x':
                running = 0; break;

            default:
                snprintf(last_message, MSG_SIZE, "Commande inconnue.");
                break;

            case 'i':
                show_history_menu(history, history_count);
                break;
        }
    }

    endwin(); // CRUCIAL : Rend le terminal normal
    printf("Retour au menu principal...\n"); // Celui-ci reste en printf car on a quitté ncurses
}
