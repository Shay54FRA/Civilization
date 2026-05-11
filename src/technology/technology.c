#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "technology.h"
#include "../game/game.h" 

// --- 1. Création de l'arbre ---
TechTree* create_tech_tree(void) {
    TechTree* tree = malloc(sizeof(TechTree));
    if (tree == NULL) return NULL;

    tree->num_technologies = 2; // On laisse 2 pour l'instant 
    tree->technologies = malloc(tree->num_technologies * sizeof(Technology));
    tree->bonus_food_forest = 0;
    tree->bonus_food_percent = 0;
    tree->bonus_gold_percent = 0;
    tree->bonus_prod_percent = 0;
    tree->bonus_science_percent = 0;
    tree->bonus_pm_units = 0;

    // Technologie 0 : L'Agriculture
    tree->technologies[0].id = 0;
    strcpy(tree->technologies[0].name, "Agriculture");
    tree->technologies[0].science_cost = 60; // Prix 
    tree->technologies[0].is_unlocked = 0;
    tree->technologies[0].num_prerequisites = 0; // Pas de prérequis
    
    // Initialisation des bonus à 0 par défaut
    tree->technologies[0].bonus.unlocked_buildings_count = 0;
    tree->technologies[0].bonus.unlocked_units_count = 0;
    tree->technologies[0].bonus.bonus_food_percent = 10; // +10% Nourriture
    tree->technologies[0].bonus.bonus_production_percent = 0;
    tree->technologies[0].bonus.bonus_science_percent = 0;
    tree->technologies[0].bonus.bonus_gold_percent = 0;
    tree->technologies[0].bonus.bonus_food_forest = 0;
    tree->technologies[0].bonus.bonus_pm_units = 0;


    // Technologie 1 : L'Élevage (Nécessite l'Agriculture)
    tree->technologies[1].id = 1;
    strcpy(tree->technologies[1].name, "Elevage");
    tree->technologies[1].science_cost = 20;
    tree->technologies[1].is_unlocked = 0;
    
    // Utilisation du tableau de prérequis
    tree->technologies[1].num_prerequisites = 1;
    tree->technologies[1].prerequisites[0] = 0; // ID de l'Agriculture
    
    // Bonus de l'Élevage
    tree->technologies[1].bonus.unlocked_buildings_count = 0;
    tree->technologies[1].bonus.unlocked_units_count = 1;
    tree->technologies[1].bonus.unlocked_units[0] = 'C'; // Cavalier
    tree->technologies[1].bonus.bonus_food_percent = 0;
    tree->technologies[1].bonus.bonus_production_percent = 0;
    tree->technologies[1].bonus.bonus_science_percent = 0;
    tree->technologies[1].bonus.bonus_gold_percent = 0;
    tree->technologies[1].bonus.bonus_food_forest = 0;
    tree->technologies[1].bonus.bonus_pm_units = 0;

    return tree;
}

// --- 2. Destruction de l'arbre ---
void destroy_tech_tree(TechTree* tree) {
    if (tree == NULL) return;
    
    free(tree->technologies);
    free(tree);
}

// --- 3. Vérifier si on PEUT lancer une recherche ---
int can_research_tech(Game* game, TechTree* tree, int tech_id) {
    if (tech_id < 0 || tech_id >= tree->num_technologies) return 0; // ID invalide
    
    Technology* tech = &tree->technologies[tech_id];

    if (tech->is_unlocked) return 0; // Déjà découverte

    // Vérification de TOUS les prérequis
    for (int i = 0; i < tech->num_prerequisites; i++) {
        int req_id = tech->prerequisites[i];
        if (tree->technologies[req_id].is_unlocked == 0) {
            return 0; // Il manque un prérequis
        }
    }

    return 1; // On peut lancer la recherche !
}

// --- 4. Mécanique de Recherche  ---

// Fonction pour choisir un nouveau projet
void set_active_research(Game* game, int tech_id) {
    if (!can_research_tech(game, game->tech_tree, tech_id)) {
        printf("-> ERREUR : Vous ne pouvez pas rechercher cette technologie (prerequis manquants).\n");
        return;
    }

    // Changer de projet en cours de route vide la cagnotte de moitié
    if (game->active_research_id != -1 && game->active_research_id != tech_id) {
        game->science /= 2; 
        printf("-> ATTENTION : Changement de projet. Votre cagnotte de science est divisee par 2 !\n");
    }

    game->active_research_id = tech_id;
    printf("-> Succes : Vous recherchez desormais '%s'.\n", game->tech_tree->technologies[tech_id].name);
}

// Fonction appelée à chaque tour par le jeu pour avancer la recherche
void update_research(Game* game) {
    if (game->active_research_id == -1) return; // Aucun projet en cours

    Technology* tech = &game->tech_tree->technologies[game->active_research_id];

    // Si la cagnotte atteint le coût
    if (game->science >= tech->science_cost) {
        tech->is_unlocked = 1;           // Technologie débloquée !
        game->science = 0;               // La cagnotte est remise à 0 (surplus perdu)
        game->active_research_id = -1;   // Le projet est terminé

        // Mise à jour des bonus globaux
        game->tech_tree->bonus_food_percent += tech->bonus.bonus_food_percent;
        game->tech_tree->bonus_prod_percent += tech->bonus.bonus_production_percent;
        game->tech_tree->bonus_gold_percent += tech->bonus.bonus_gold_percent;
        game->tech_tree->bonus_science_percent += tech->bonus.bonus_science_percent;

        game->tech_tree->bonus_food_forest += tech->bonus.bonus_food_forest;
        game->tech_tree->bonus_pm_units += tech->bonus.bonus_pm_units;
        
        printf("\n*** DECOUVERTE : Vous avez decouvert '%s' ! ***\n", tech->name);
    }
}

// --- 5. L'interface Menu ---
void show_technology_menu(Game* game) {
    int choix = 0;
    
    while (choix != -1) {
        printf("\n=========================================\n");
        printf("        ARBRE DES TECHNOLOGIES           \n");
        
        // Affichage du projet en cours
        if (game->active_research_id != -1) {
            Technology active = game->tech_tree->technologies[game->active_research_id];
            printf(" PROJET ACTIF : %s (%d / %d Science)\n", active.name, game->science, active.science_cost);
        } else {
            printf(" PROJET ACTIF : Aucun (Science perdue !)\n");
        }
        printf("=========================================\n");

        // Liste des technos
        for(int i = 0; i < game->tech_tree->num_technologies; i++) {
            Technology t = game->tech_tree->technologies[i];
            
            char status[30];
            if (t.is_unlocked) strcpy(status, "[DECOUVERT]");
            else if (game->active_research_id == t.id) strcpy(status, "[EN COURS]");
            else if (can_research_tech(game, game->tech_tree, t.id)) strcpy(status, "[DISPONIBLE]");
            else strcpy(status, "[BLOQUE]");

            printf(" ID: %d | %s (Cout: %d) %s\n", t.id, t.name, t.science_cost, status);
        }
        
        printf("\nEntrez l'ID pour lancer une recherche (ou -1 pour revenir) : ");
        
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n'); 
            continue; 
        }

        if (choix != -1) {
            set_active_research(game, choix);
        }
    }
}

// --- 6. Fonctions utilitaires ---
int is_building_unlocked(TechTree* tree, char building_type) {
    if (tree == NULL) return 0;
    for (int i = 0; i < tree->num_technologies; i++) {
        if (tree->technologies[i].is_unlocked) {
            for (int j = 0; j < tree->technologies[i].bonus.unlocked_buildings_count; j++) {
                if (tree->technologies[i].bonus.unlocked_buildings[j] == building_type) return 1;
            }
        }
    }
    return 0;
}

int is_unit_unlocked(TechTree* tree, char unit_type) {
    if (tree == NULL) return 0;
    for (int i = 0; i < tree->num_technologies; i++) {
        if (tree->technologies[i].is_unlocked) {
            for (int j = 0; j < tree->technologies[i].bonus.unlocked_units_count; j++) {
                if (tree->technologies[i].bonus.unlocked_units[j] == unit_type) return 1;
            }
        }
    }
    return 0;
}