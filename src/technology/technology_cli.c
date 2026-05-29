#include <stdio.h>
#include "technology.h"
#include "../game/game.h"
#include "../cli/cli.h"
#include "../map/map.h"
#include <ncurses.h>

/*
========================================
 AFFICHAGE ARBRE TECHNOLOGIQUE
========================================
*/
void print_progress_bar(int current, int total, int width);

void print_tech_tree_cli(TechTree* tree, int active_research_id, Game* game)
{
    if (!tree || !game) return;

    attron(COLOR_PAIR(TEXTE_BLEU));
    printw("\n========== ARBRE TECHNOLOGIQUE ==========\n");
    attroff(COLOR_PAIR(TEXTE_BLEU));

    for (int i = 0; i < tree->num_technologies; i++)
    {
        Technology* tech = &tree->technologies[i];

        /* cacher technologie de Départ */
        if (tech->id == 0)
            continue;

        printw("\n[%d] %s ", tech->id, tech->name);

        /* déjà débloquée */
        if (tech->is_unlocked)
        {
            attron(COLOR_PAIR(TEXTE_VERT));
            printw("OK");
            attroff(COLOR_PAIR(TEXTE_VERT));
        }
        /* recherche en cours */
        else if (tech->id == active_research_id)
        {
            int progress = game->science;
            int cost = tech->science_cost;
            printw(" ");
            print_progress_bar(progress, cost, 15);
        }
        /* prérequis non validés */
        else if (!can_research_tech(game, tree, tech->id))
        {
            attron(COLOR_PAIR(COLOR_ROUGE));
            printw("BLOQUEE");
            attroff(COLOR_PAIR(COLOR_ROUGE));
        }
        /* recherchable */
        else
        {
            attron(COLOR_PAIR(TEXTE_ORANGE));
            printw("(%d science)", tech->science_cost);
            attroff(COLOR_PAIR(TEXTE_ORANGE));
        }
    }

    attron(COLOR_PAIR(TEXTE_BLEU));
    printw("\n\n=========================================\n");
    attroff(COLOR_PAIR(TEXTE_BLEU));
}


/*
========================================
 TECHNOLOGIES DISPONIBLES
========================================
*/

void print_available_techs_cli(Game* game)
{
    if (!game || !game->tech_tree) return;

    TechTree* tree = game->tech_tree;

    attron(COLOR_PAIR(TEXTE_ROSE));
    printw("\nTechnologies recherchables :\n");
    attroff(COLOR_PAIR(TEXTE_ROSE));

    for (int i = 0; i < tree->num_technologies; i++)
    {
        Technology* tech = &tree->technologies[i];

        if (tech->id == 0)
            continue;

        if (!tech->is_unlocked &&
            can_research_tech(game, tree, tech->id))
        {
            attron(COLOR_PAIR(TEXTE_ORANGE));
            printw(" -> [%d] %s (%d science)\n",
                   tech->id,
                   tech->name,
                   tech->science_cost);
            attroff(COLOR_PAIR(TEXTE_ORANGE));
        }
    }
}


/*
========================================
 MENU TECHNOLOGIE
========================================
*/

void show_technology_menu(Game* game)
{
    if (!game || !game->tech_tree)
        return;

    int choix;

    while (1)
    {
        clear();
        refresh();
        
        attron(COLOR_PAIR(TEXTE_ROSE));
        printw("\n===== MENU TECHNOLOGIES =====\n");
        attroff(COLOR_PAIR(TEXTE_ROSE));

        int active_id = game->active_research_id;

        print_tech_tree_cli(game->tech_tree, active_id, game);
        print_available_techs_cli(game);

        printw("\nChoisir une technologie (-1 pour quitter) : ");
        scanw("%d", &choix);

        if (choix == -1)
            break;

        attempt_research(game, choix);
    }
}

/*
========================================
 BARRE DE PROGRESSION
========================================
*/


void print_progress_bar(int current, int total, int width)
{
    if (total <= 0) return;

    float ratio = (float)current / (float)total;
    int filled = (int)(ratio * width);

    // Choix couleur selon progression
    if (ratio < 0.3f)
        attron(COLOR_PAIR(COLOR_ROUGE)); // rouge
    else if (ratio < 0.7f)
        attron(COLOR_PAIR(COLOR_JAUNE)); // jaune
    else
        attron(COLOR_PAIR(COLOR_VERT)); // vert

    printw("[");

    for (int i = 0; i < width; i++)
    {
        if (i < filled)
            printw("█");
        else
            printw("-");
    }

    printw("]");
    
    if (ratio < 0.3f)
        attroff(COLOR_PAIR(COLOR_ROUGE));
    else if (ratio < 0.7f)
        attroff(COLOR_PAIR(COLOR_JAUNE));
    else
        attroff(COLOR_PAIR(COLOR_VERT));

    printw(" %d / %d", current, total);
}
