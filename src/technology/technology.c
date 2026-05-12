#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "technology.h"
#include "../game/game.h" 

// --- 1. Création de l'arbre ---
TechTree* create_tech_tree(void) {
    TechTree* tree = malloc(sizeof(TechTree));
    if (tree == NULL) return NULL;

    tree->num_technologies = 9; // Le noeud Départ + Les 8 technologies du CDC
    tree->technologies = malloc(tree->num_technologies * sizeof(Technology));
    tree->bonus_food_forest = 0;
    tree->bonus_food_percent = 0;
    tree->bonus_gold_percent = 0;
    tree->bonus_prod_percent = 0;
    tree->bonus_science_percent = 0;
    tree->bonus_pm_units = 0;

    // Initialisation des tableaux de technologies à 0 (rien n'est débloqué au début)
    for(int i = 0; i < 256; i++) {
        tree->unlocked_buildings_global[i] = 0;
        tree->unlocked_units_global[i] = 0;
    }

    // Remise à zéro de tous les bonus pour éviter les bugs mémoire
    for(int i = 0; i < tree->num_technologies; i++) {
        tree->technologies[i].bonus.unlocked_buildings_count = 0;
        tree->technologies[i].bonus.unlocked_units_count = 0;
        tree->technologies[i].bonus.bonus_food_percent = 0;
        tree->technologies[i].bonus.bonus_production_percent = 0;
        tree->technologies[i].bonus.bonus_science_percent = 0;
        tree->technologies[i].bonus.bonus_gold_percent = 0;
        tree->technologies[i].bonus.bonus_food_forest = 0;
        tree->technologies[i].bonus.bonus_pm_units = 0;
    }

    // Technologie 0 : DÉPART (Déjà débloqué, sert de racine)
    tree->technologies[0].id = 0;
    strcpy(tree->technologies[0].name, "Depart");
    tree->technologies[0].science_cost = 0;
    tree->technologies[0].is_unlocked = 1; 
    tree->technologies[0].num_prerequisites = 0;

    // Technologie 1 : CHASSE
    tree->technologies[1].id = 1;
    strcpy(tree->technologies[1].name, "Chasse");
    tree->technologies[1].science_cost = 50;
    tree->technologies[1].is_unlocked = 0;
    tree->technologies[1].num_prerequisites = 1;
    tree->technologies[1].prerequisites[0] = 0; // Départ
    tree->technologies[1].bonus.bonus_food_forest = 1;

    // Technologie 2 : AGRICULTURE
    tree->technologies[2].id = 2;
    strcpy(tree->technologies[2].name, "Agriculture");
    tree->technologies[2].science_cost = 60;
    tree->technologies[2].is_unlocked = 0;
    tree->technologies[2].num_prerequisites = 1;
    tree->technologies[2].prerequisites[0] = 0; // Départ
    tree->technologies[2].bonus.bonus_food_percent = 10;

    // Technologie 3 : ARTISANAT
    tree->technologies[3].id = 3;
    strcpy(tree->technologies[3].name, "Artisanat");
    tree->technologies[3].science_cost = 70;
    tree->technologies[3].is_unlocked = 0;
    tree->technologies[3].num_prerequisites = 1;
    tree->technologies[3].prerequisites[0] = 0; // Départ
    tree->technologies[3].bonus.bonus_production_percent = 10;
    tree->technologies[3].bonus.unlocked_units[0] = 'g'; // Débloque Guerrier
    tree->technologies[3].bonus.unlocked_units_count = 1;

    // Technologie 4 : ÉCRITURE
    tree->technologies[4].id = 4;
    strcpy(tree->technologies[4].name, "Ecriture");
    tree->technologies[4].science_cost = 80;
    tree->technologies[4].is_unlocked = 0;
    tree->technologies[4].num_prerequisites = 1;
    tree->technologies[4].prerequisites[0] = 0; // Départ
    tree->technologies[4].bonus.bonus_science_percent = 10;
    tree->technologies[4].bonus.unlocked_buildings[0] = 'B'; // Débloque Bibliothèque
    tree->technologies[4].bonus.unlocked_buildings_count = 1;

    // Technologie 5 : ÉQUITATION
    tree->technologies[5].id = 5;
    strcpy(tree->technologies[5].name, "Equitation");
    tree->technologies[5].science_cost = 100;
    tree->technologies[5].is_unlocked = 0;
    tree->technologies[5].num_prerequisites = 1;
    tree->technologies[5].prerequisites[0] = 0; // Départ
    tree->technologies[5].bonus.bonus_pm_units = 1;

    // Technologie 6 : IRRIGATION
    tree->technologies[6].id = 6;
    strcpy(tree->technologies[6].name, "Irrigation");
    tree->technologies[6].science_cost = 90;
    tree->technologies[6].is_unlocked = 0;
    tree->technologies[6].num_prerequisites = 1;
    tree->technologies[6].prerequisites[0] = 2; // Nécessite Agriculture
    tree->technologies[6].bonus.bonus_food_percent = 20;

    // Technologie 7 : MAÇONNERIE
    tree->technologies[7].id = 7;
    strcpy(tree->technologies[7].name, "Maconnerie");
    tree->technologies[7].science_cost = 100;
    tree->technologies[7].is_unlocked = 0;
    tree->technologies[7].num_prerequisites = 1;
    tree->technologies[7].prerequisites[0] = 3; // Nécessite Artisanat
    tree->technologies[7].bonus.unlocked_buildings[0] = 'M'; // Débloque Muraille
    tree->technologies[7].bonus.unlocked_buildings_count = 1;

    // Technologie 8 : COMMERCE
    tree->technologies[8].id = 8;
    strcpy(tree->technologies[8].name, "Commerce");
    tree->technologies[8].science_cost = 90;
    tree->technologies[8].is_unlocked = 0;
    tree->technologies[8].num_prerequisites = 2;
    tree->technologies[8].prerequisites[0] = 3; // Nécessite Artisanat
    tree->technologies[8].prerequisites[1] = 4; // ET Écriture
    tree->technologies[8].bonus.bonus_gold_percent = 10;
    tree->technologies[8].bonus.unlocked_buildings[0] = 'C'; // Débloque Marché
    tree->technologies[8].bonus.unlocked_buildings_count = 1;

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

        // --- MISE A JOUR DES TABLEAUX DE TECHNOLOGIES ---
        for (int i = 0; i < tech->bonus.unlocked_buildings_count; i++) {
            char b = tech->bonus.unlocked_buildings[i];
            game->tech_tree->unlocked_buildings_global[(unsigned char)b] = 1;
        }
        for (int i = 0; i < tech->bonus.unlocked_units_count; i++) {
            char u = tech->bonus.unlocked_units[i];
            game->tech_tree->unlocked_units_global[(unsigned char)u] = 1;
        }
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

        // Liste des technos (on commence à 1 pour cacher le noeud "Depart")
        for(int i = 1; i < game->tech_tree->num_technologies; i++) {
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
    return tree->unlocked_buildings_global[(unsigned char)building_type];
}

int is_unit_unlocked(TechTree* tree, char unit_type) {
    if (tree == NULL) return 0;
    return tree->unlocked_units_global[(unsigned char)unit_type];
}