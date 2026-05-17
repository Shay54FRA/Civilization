#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "technology.h"
#include "../game/game.h"
#include "../unit/unit.h"


// =======================================================
// 1. CREATION DE L'ARBRE
// =======================================================

TechTree* create_tech_tree(void)
{
    TechTree* tree = malloc(sizeof(TechTree));
    if (!tree) return NULL;

    tree->num_technologies = 9;

    tree->technologies =
        calloc(tree->num_technologies, sizeof(Technology));

    if (!tree->technologies) {
        free(tree);
        return NULL;
    }

    tree->num_unlocked = 0;

    /* BONUS GLOBAUX */
    tree->bonus_food_forest = 0;
    tree->bonus_food_percent = 0;
    tree->bonus_gold_percent = 0;
    tree->bonus_prod_percent = 0;
    tree->bonus_science_percent = 0;
    tree->bonus_pm_units = 0;

    memset(tree->unlocked_buildings_global, 0,
           sizeof(tree->unlocked_buildings_global));

    memset(tree->unlocked_units_global, 0,
           sizeof(tree->unlocked_units_global));


    // ---------------------------------------------------
    // TECHNO 0 : DEPART
    // ---------------------------------------------------
    Technology* t = &tree->technologies[0];

    t->id = 0;
    strcpy(t->name, "Depart");
    t->science_cost = 0;
    t->is_unlocked = 1;
    t->num_prerequisites = 0;


    // ---------------------------------------------------
    // CHASSE
    // ---------------------------------------------------
    t = &tree->technologies[1];

    t->id = 1;
    strcpy(t->name, "Chasse");
    t->science_cost = 50;
    t->num_prerequisites = 1;
    t->prerequisites[0] = 0;
    t->bonus.bonus_food_forest = 1;


    // ---------------------------------------------------
    // AGRICULTURE
    // ---------------------------------------------------
    t = &tree->technologies[2];

    t->id = 2;
    strcpy(t->name, "Agriculture");
    t->science_cost = 60;
    t->num_prerequisites = 1;
    t->prerequisites[0] = 0;
    t->bonus.bonus_food_percent = 10;


    // ---------------------------------------------------
    // ARTISANAT
    // ---------------------------------------------------
    t = &tree->technologies[3];

    t->id = 3;
    strcpy(t->name, "Artisanat");
    t->science_cost = 70;
    t->num_prerequisites = 1;
    t->prerequisites[0] = 0;

    t->bonus.bonus_production_percent = 10;
    t->bonus.unlocked_units[0] = 'g';
    t->bonus.unlocked_units_count = 1;


    // ---------------------------------------------------
    // ECRITURE
    // ---------------------------------------------------
    t = &tree->technologies[4];

    t->id = 4;
    strcpy(t->name, "Ecriture");
    t->science_cost = 80;
    t->num_prerequisites = 1;
    t->prerequisites[0] = 0;

    t->bonus.bonus_science_percent = 10;
    t->bonus.unlocked_buildings[0] = 'B';
    t->bonus.unlocked_buildings_count = 1;


    // ---------------------------------------------------
    // EQUITATION
    // ---------------------------------------------------
    t = &tree->technologies[5];

    t->id = 5;
    strcpy(t->name, "Equitation");
    t->science_cost = 100;
    t->num_prerequisites = 1;
    t->prerequisites[0] = 0;

    t->bonus.bonus_pm_units = 1;


    // ---------------------------------------------------
    // IRRIGATION
    // ---------------------------------------------------
    t = &tree->technologies[6];

    t->id = 6;
    strcpy(t->name, "Irrigation");
    t->science_cost = 90;
    t->num_prerequisites = 1;
    t->prerequisites[0] = 2;

    t->bonus.bonus_food_percent = 20;


    // ---------------------------------------------------
    // MACONNERIE
    // ---------------------------------------------------
    t = &tree->technologies[7];

    t->id = 7;
    strcpy(t->name, "Maconnerie");
    t->science_cost = 100;
    t->num_prerequisites = 1;
    t->prerequisites[0] = 3;

    t->bonus.unlocked_buildings[0] = 'R';
    t->bonus.unlocked_buildings_count = 1;


    // ---------------------------------------------------
    // COMMERCE
    // ---------------------------------------------------
    t = &tree->technologies[8];

    t->id = 8;
    strcpy(t->name, "Commerce");
    t->science_cost = 90;
    t->num_prerequisites = 2;
    t->prerequisites[0] = 3;
    t->prerequisites[1] = 4;

    t->bonus.bonus_gold_percent = 10;
    t->bonus.unlocked_buildings[0] = 'M';
    t->bonus.unlocked_buildings_count = 1;

    return tree;
}


// =======================================================
// 2. DESTRUCTION
// =======================================================

void destroy_tech_tree(TechTree* tree)
{
    if (!tree) return;

    free(tree->technologies);
    free(tree);
}


// =======================================================
// 3. VERIFICATION PREREQUIS
// =======================================================

int can_research_tech(Game* game, TechTree* tree, int tech_id)
{
    (void)game;

    if (!tree) return 0;
    if (tech_id < 0 || tech_id >= tree->num_technologies)
        return 0;

    Technology* tech = &tree->technologies[tech_id];

    if (tech->is_unlocked)
        return 0;

    for (int i = 0; i < tech->num_prerequisites; i++) {
        int req = tech->prerequisites[i];

        if (!tree->technologies[req].is_unlocked)
            return 0;
    }

    return 1;
}


// =======================================================
// 4. RECHERCHE
// =======================================================

int set_active_research(Game* game, int tech_id)
{
    if (!can_research_tech(game, game->tech_tree, tech_id))
        return -1;

    if (game->active_research_id != -1 &&
        game->active_research_id != tech_id)
    {
        game->science /= 2;
    }

    game->active_research_id = tech_id;
    return 0;
}


int update_research(Game* game)
{
    if (game->active_research_id == -1)
        return -1;

    Technology* tech =
        &game->tech_tree->technologies[game->active_research_id];

    if (game->science < tech->science_cost)
        return -1;

    tech->is_unlocked = 1;

    game->science = 0;
    game->active_research_id = -1;

    TechTree* tree = game->tech_tree;

    tree->bonus_food_percent += tech->bonus.bonus_food_percent;
    tree->bonus_prod_percent += tech->bonus.bonus_production_percent;
    tree->bonus_gold_percent += tech->bonus.bonus_gold_percent;
    tree->bonus_science_percent += tech->bonus.bonus_science_percent;
    tree->bonus_food_forest += tech->bonus.bonus_food_forest;
    tree->bonus_pm_units += tech->bonus.bonus_pm_units;
    tree->num_unlocked += 1;

    if (tech->bonus.bonus_pm_units > 0) {
        UnitList* current = game->unitList;

        while (current != NULL) {
            if (current->data != NULL) {
                current->data->max_pm += tech->bonus.bonus_pm_units;
                current->data->pm += tech->bonus.bonus_pm_units;
            }

            current = current->next;
        }
    }

    for(int i=0;i<tech->bonus.unlocked_buildings_count;i++)
        tree->unlocked_buildings_global[
            (unsigned char)tech->bonus.unlocked_buildings[i]] = 1;

    for(int i=0;i<tech->bonus.unlocked_units_count;i++)
        tree->unlocked_units_global[
            (unsigned char)tech->bonus.unlocked_units[i]] = 1;

    return tech->id;
}


void attempt_research(Game* game, int tech_id)
{
    set_active_research(game, tech_id);
}


// =======================================================
// 5. UTILITAIRES
// =======================================================

int is_building_unlocked(TechTree* tree, char type)
{
    if (!tree) return 0;
    return tree->unlocked_buildings_global[(unsigned char)type];
}

int is_unit_unlocked(TechTree* tree, char type)
{
    if (!tree) return 0;
    return tree->unlocked_units_global[(unsigned char)type];
}
