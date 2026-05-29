#include <stdio.h>
#include <ncurses.h>
#include "cli_panneaux.h"
#include "../tile/tile.h"
#include "../city/city.h"
#include "../barbarian/barbarian.h"
#include "../building/building.h"
#include <string.h>

void print_pos(WINDOW* win,Position pos) {
    wprintw(win,"Position : (%d, %d)", pos.x, pos.y);
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
void print_tile_info(WINDOW* win,Game* game, Position cursor) {
    Tile* tile = get_tile(game->map, cursor);

    if (!tile) return;

    int cost = terrain_move_cost(tile->biome);

    wprintw(win,"\n=== CASE SELECTIONNEE ===\n");

    // Brouillard complet
    if (tile->fog_level == 0) {
        wprintw(win, "\nBrouillard : Aucune info sur la case !\n");
        wprintw(win,"Position : (%d, %d)\n", cursor.x, cursor.y);
        return;
    }

    wprintw(win,"Position : (%d, %d)\n", cursor.x, cursor.y);

    int biome_color = COLOR_PLAINE; // Couleur par défaut
    switch(tile->biome) {
        case 'E': biome_color = COLOR_EAU; break;
        case 'P': biome_color = COLOR_PLAINE; break;
        case 'F': biome_color = COLOR_FORET; break;
        case 'M': biome_color = COLOR_MONTAGNE; break;
        case 'D': biome_color = COLOR_DESERT; break;
        case 'T': biome_color = COLOR_TOUNDRA; break;
    }
    wprintw(win,"Terrain  : ");
    wattron(win, COLOR_PAIR(biome_color));
    wprintw(win,"%s\n", biome_name(tile->biome));
    wattroff(win, COLOR_PAIR(biome_color));


    char l2[50], l3[50], l4[50];

    // Statistiques des biomes
    switch (tile->biome) {
        case 'P': // Plaine
            strcpy(l2, "Nourriture : +2"); strcpy(l3, "Production : +1"); strcpy(l4, "PM requis  : 1"); break;
        case 'F': // Forêt
            // sprintf(l2, "Nourriture : %+d", 1 + game->tech_tree->bonus_food_forest); 
            strcpy(l3, "Production : +2"); strcpy(l4, "PM requis  : 2"); break;
        case 'M': // Montagne
            strcpy(l2, "Production : +3"); strcpy(l3, "Science    : +1"); strcpy(l4, "PM requis  : 3"); break;
        case 'E': // Eau
            strcpy(l2, "Nourriture : +1"); strcpy(l3, "Or         : +1"); strcpy(l4, "PM requis  : Bloque"); break;
        case 'D': // Désert
            strcpy(l2, "Nourriture : 0");  strcpy(l3, "Or         : +1"); strcpy(l4, "PM requis  : 1"); break;
        case 'T': // Toundra
            strcpy(l2, "Nourriture : +1"); strcpy(l3, "Production : +1"); strcpy(l4, "PM requis  : 1"); break;
        default:
            strcpy(l2, "Nourriture : 0");  strcpy(l3, "Production : 0");  strcpy(l4, "PM requis  : 1"); break;
    }

    wattron(win,COLOR_PAIR(TEXTE_VERT));
    wprintw(win,"%s\n",l2); //affiche Production 
    wattroff(win,COLOR_PAIR(TEXTE_VERT));
    wattron(win,COLOR_PAIR(TEXTE_BLEU));
    wprintw(win,"%s\n",l3); //affiche Nourriture
    wattroff(win,COLOR_PAIR(TEXTE_VERT));
    wattron(win,COLOR_PAIR(TEXTE_ORANGE));
    wprintw(win,"%s\n",l4); //affiche PM requis
    wattroff(win,COLOR_PAIR(TEXTE_ORANGE));



    if (cost == -1)
        wprintw(win,"Cout PM  : Infranchissable\n");
    else
        wprintw(win,"Cout PM  : %d\n", cost);

    if (tile->city_on)
        wprintw(win,"Contenu  : Ville\n");

    else if (tile->unit)
        wprintw(win,"Contenu  : Unite %s [%c]\n", get_name(tile->unit->type), tile->unit->type);

    else if (tile->fog_level == 1) {
        wprintw(win, "Contenu  : Brouillard trop fort !");
    }

    else if (tile->barb_on) {
        wprintw(win,"Contenu  : Barbare | %dpv - %datk - %ddef\n", tile->barb_on->pv, tile->barb_on->atk, tile->barb_on->def);
    }

    else if (tile->camp_on) {
        wprintw(win,"Contenu  : Camp de barbares\n");
    }

    else
        wprintw(win,"Contenu  : Vide\n");
}


// Affiche les statistiques de l’unité sélectionnée
void print_selected_unit_info(WINDOW* win,Unit* selected_unit) {
    wprintw(win,"\n=== UNITE SELECTIONNEE ===\n");

    if (!selected_unit) {
        wprintw(win,"Aucune unite selectionnee.\n");
        return;
    }

    wprintw(win,"Type : %s [%c]\n", get_name(selected_unit->type), selected_unit->type);

    wprintw(win,"PV   : %d / %d\n", selected_unit->pv, selected_unit->max_pv);

    wprintw(win,"PM   : %d / %d\n", selected_unit->pm, selected_unit->max_pm);

    wprintw(win,"ATK  : %d\n", selected_unit->atk);
    wprintw(win,"DEF  : %d\n", selected_unit->def);

    wprintw(win,"Pos  : (%d, %d)\n", selected_unit->pos.x, selected_unit->pos.y);
}

void show_city_info_cli(WINDOW* win,Game* game, Position pos) {
    if (game == NULL) return;
    Tile* tile = get_tile(game->map, pos);
    if (tile == NULL) return;
    City* city = get_city_on_tile(game->cityList, tile);
    if (city == NULL) {
        wprintw(win,"Aucune ville séléctionné !\n");
        return;
    }
    wattron(win,COLOR_PAIR(TEXTE_ROSE));
    wprintw(win,"\n==== VILLE ====\n\n");
    wprintw(win,"PV : %d / %d\n", get_city_pv(city), MAX_HP);
    wprintw(win,"Population : %d villageois\n", city->population);
    wprintw(win,"Force de défense : %d\n", city->strength);
    wprintw(win,"Nourriture : %d\n", city->food);
    wprintw(win,"Production : %d\n", city->production);

    wprintw(win,"\n--- Projet ---\n\n");
    if (city->project == NULL) {
        wprintw(win,"Aucun projet en cours ! Les points de productions sont perdus à chaque tour.\n");
    } else {
        wprintw(win,"Type : %s\n", get_name(city->project->type));
        wprintw(win,"Production nécéssaire restante : %d\n", city->project->production_cost);
    }

    wprintw(win,"\n--- Batîments ---\n\n");
    BuildList* to_check = city->buildings;
    while(to_check != NULL) {
        Building* build = to_check->data;
        if (build != NULL) {
            wprintw(win,"%s\n", get_name(build->type));
        }
        to_check = to_check->next;
    }

    wattroff(win,COLOR_PAIR(TEXTE_ROSE)); //On reset la couleur de fond
    return;

}

void print_stats(WINDOW* win,Game* game) {
    if (!game) return;

    // Calcul de la population
    int total_pop = get_total_pop(game->cityList);
    int city_count = get_city_number(game);

    // Calcul des unites
    int unit_count = get_unit_number(game);
    int maintenance = get_all_entretien_costs(game);
    // UnitList* current_unit = game->unitList;

    
    Position start_pos = get_starting_city_pos(game->map);
    

    // Menu des stats
    wprintw(win,"\n=== TOUR %d | Or: %d | Science: %d ===\n", game->active_turn, game->gold, game->science);
    wprintw(win,"Empire : %d Villes (%d Pop) | Armee : %d Unites (Entretien: -%d Or/tour)\n", 
           city_count, total_pop, unit_count, maintenance);
    wattron(win,COLOR_PAIR(COLOR_VILLE));
    wprintw(win,"Ville de départ : (%d, %d)\n", start_pos.x, start_pos.y);
    wattroff(win,COLOR_PAIR(COLOR_VILLE)); //On reset la couleur de fond
}


void print_action_help(WINDOW* win) {

    wprintw(win,"\nCommandes : [z/q/s/d] Déplacer caméra | [m] Sélectionner/Déplacer unité | [v] Fonder ville | [t] Technologies | [r] Projets de ville | [f] Fin de tour | [i] Historique | [x] Quitter\n");
    wprintw(win,"> ");

}
