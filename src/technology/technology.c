#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "technology.h"
#include "../game/game.h" 

// --- 1. Création de l'arbre  ---
TechTree* create_tech_tree(void) {
    TechTree* tree = malloc(sizeof(TechTree));
    if (tree == NULL) return NULL;

    tree->num_technologies = 2; // On en met 2 pour l'exemple
    tree->technologies = malloc(tree->num_technologies * sizeof(Technology));

    // Technologie 0 : L'Agriculture (Racine, pas de prérequis)
    tree->technologies[0].id = 0;
    strcpy(tree->technologies[0].name, "Agriculture");
    strcpy(tree->technologies[0].description, "Permet de cultiver la terre.");
    tree->technologies[0].science_cost = 10;
    tree->technologies[0].is_unlocked = 0;
    tree->technologies[0].num_prerequisites = 0;
    tree->technologies[0].prerequisite_ids = NULL;
    // On débloque le Grenier ('G')
    tree->technologies[0].bonus.unlocked_buildings_count = 1;
    tree->technologies[0].bonus.unlocked_buildings = malloc(1 * sizeof(char));
    tree->technologies[0].bonus.unlocked_buildings[0] = 'G';
    tree->technologies[0].bonus.unlocked_units_count = 0;
    tree->technologies[0].bonus.unlocked_units = NULL;

    // Technologie 1 : L'Élevage (Nécessite l'Agriculture)
    tree->technologies[1].id = 1;
    strcpy(tree->technologies[1].name, "Elevage");
    strcpy(tree->technologies[1].description, "Permet d'utiliser les chevaux.");
    tree->technologies[1].science_cost = 20;
    tree->technologies[1].is_unlocked = 0;
    tree->technologies[1].num_prerequisites = 1;
    tree->technologies[1].prerequisite_ids = malloc(1 * sizeof(int));
    tree->technologies[1].prerequisite_ids[0] = 0; // ID de l'Agriculture
    // On débloque le Cavalier ('C')
    tree->technologies[1].bonus.unlocked_buildings_count = 0;
    tree->technologies[1].bonus.unlocked_buildings = NULL;
    tree->technologies[1].bonus.unlocked_units_count = 1;
    tree->technologies[1].bonus.unlocked_units = malloc(1 * sizeof(char));
    tree->technologies[1].bonus.unlocked_units[0] = 'C';

    return tree;
}

// --- 2. Destruction de l'arbre ---
void destroy_tech_tree(TechTree* tree) {
    if (tree == NULL) return;

    for (int i = 0; i < tree->num_technologies; i++) {
        if (tree->technologies[i].prerequisite_ids != NULL) free(tree->technologies[i].prerequisite_ids);
        if (tree->technologies[i].bonus.unlocked_units != NULL) free(tree->technologies[i].bonus.unlocked_units);
        if (tree->technologies[i].bonus.unlocked_buildings != NULL) free(tree->technologies[i].bonus.unlocked_buildings);
    }
    
    free(tree->technologies);
    free(tree);
}

// --- 3. Vérifier si on PEUT débloquer une technologie ---
int can_unlock_tech(Game* game, TechTree* tree, int tech_id) {
    if (tech_id < 0 || tech_id >= tree->num_technologies) return 0; // ID invalide
    
    Technology* tech = &tree->technologies[tech_id];

    if (tech->is_unlocked) return 0; // Déjà débloquée
    if (game->science < tech->science_cost) return 0; // Pas assez de science

    // Vérification des prérequis
    for (int i = 0; i < tech->num_prerequisites; i++) {
        int req_id = tech->prerequisite_ids[i];
        if (tree->technologies[req_id].is_unlocked == 0) {
            return 0; // Il manque un prérequis
        }
    }

    return 1; // Tout est bon, on peut débloquer !
}

// --- 4. Débloquer la technologie ---
void unlock_tech(Game* game, TechTree* tree, int tech_id) {
    if (can_unlock_tech(game, tree, tech_id)) {
        Technology* tech = &tree->technologies[tech_id];
        
        // On paye le coût
        game->science -= tech->science_cost;
        
        // On la marque comme débloquée
        tech->is_unlocked = 1;
        
        printf("Succès : Technologie '%s' débloquée !\n", tech->name);
        
        // Ajout information de construction ultérieure
    } else {
        printf("Erreur : Impossible de débloquer la technologie.\n");
    }
}

void show_technology_menu(Game* game) {
    int choix = 0;
    
    // On boucle tant que le joueur ne tape pas -1 pour quitter le menu
    while (choix != -1) {
        printf("\n=========================================\n");
        printf("        ARBRE DES TECHNOLOGIES           \n");
        printf("        Votre Science : %d               \n", game->science);
        printf("=========================================\n");

        // On liste toutes les technos
        for(int i = 0; i < game->tech_tree->num_technologies; i++) {
            Technology t = game->tech_tree->technologies[i];
            
            // On affiche un petit tag pour savoir si c'est débloqué ou non
            char status[20];
            if (t.is_unlocked) strcpy(status, "[DEBLOQUE]");
            else if (can_unlock_tech(game, game->tech_tree, t.id)) strcpy(status, "[DISPONIBLE]");
            else strcpy(status, "[BLOQUE]");

            printf(" ID: %d | %s (Cout: %d) %s\n", t.id, t.name, t.science_cost, status);
        }
        
        printf("\nEntrez l'ID de la technologie a acheter (ou -1 pour revenir a la carte) : ");
        
        // Sécurité : si le joueur tape une lettre au lieu d'un chiffre
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n'); // On vide le buffer pour éviter une boucle infinie
            continue; 
        }

        // Si le joueur a tapé un ID valide, on essaie de débloquer
        if (choix != -1) {
            if (can_unlock_tech(game, game->tech_tree, choix)) {
                unlock_tech(game, game->tech_tree, choix);
                // Le message de succès est déjà géré dans unlock_tech()
            } else {
                printf("-> ERREUR : Vous ne pouvez pas acheter cette technologie (Fonds insuffisants, deja possedee ou prerequis manquants).\n");
            }
        }
    }
}

// --- 5. Fonctions utilitaires pour le reste du jeu ---

int is_building_unlocked(TechTree* tree, char building_type) {
    if (tree == NULL) return 0;

    for (int i = 0; i < tree->num_technologies; i++) {
        Technology* t = &tree->technologies[i];
        
        if (t->is_unlocked) {
            for (int j = 0; j < t->bonus.unlocked_buildings_count; j++) {
                if (t->bonus.unlocked_buildings[j] == building_type) {
                    return 1; // Vrai : le bâtiment est débloqué !
                }
            }
        }
    }
    return 0; // Faux : non trouvé dans les technos possédées
}

int is_unit_unlocked(TechTree* tree, char unit_type) {
    if (tree == NULL) return 0;

    for (int i = 0; i < tree->num_technologies; i++) {
        Technology* t = &tree->technologies[i];
        
        if (t->is_unlocked) {
            for (int j = 0; j < t->bonus.unlocked_units_count; j++) {
                if (t->bonus.unlocked_units[j] == unit_type) {
                    return 1; // Vrai : l'unité est débloquée !
                }
            }
        }
    }
    return 0; // Faux
}