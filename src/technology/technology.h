#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H

// On inclut game.h pour pouvoir modifier la science plus tard si besoin
typedef struct _Game Game; 

/* 
 * 1. La "plus petite structure" pour gérer les bonus.
 */
typedef struct {
    int unlocked_units_count;
    char* unlocked_units;       // Ex: ['A', 'C'] pour débloquer Archer et Cavalier
    
    int unlocked_buildings_count;
    char* unlocked_buildings;   // Ex: ['G', 'M'] pour Grenier et Muraille
    
    // On pourra ajouter d'autres bonus plus tard
} TechBonus;

/*
 * 2. Le noeud principal : la Technologie
 */
typedef struct _Technology {
    int id;                     // Identifiant unique (ex: 1)
    char name[64];              // Nom (ex: "Poterie")
    char description[256];      // Petite description du lore ou de l'effet
    
    int science_cost;           // Le coût à payer en Game->science
    int is_unlocked;            // 0 = non possédée, 1 = possédée
    
    int num_prerequisites;
    int* prerequisite_ids;      // Tableau des IDs des technos requises pour la débloquer
    
    TechBonus bonus;            // Ce qu'elle débloque
} Technology;

/*
 * 3. L'arbre complet qui regroupe tout
 */
typedef struct _TechTree {
    int num_technologies;
    Technology* technologies;   // Tableau dynamique contenant toutes les technos du jeu
} TechTree;

// --- Fonctions principales ---

// Créer et détruire l'arbre
TechTree* create_tech_tree(void);
void destroy_tech_tree(TechTree* tree);

// Fonctions de logique de jeu
int can_unlock_tech(Game* game, TechTree* tree, int tech_id); // Vérifie la science ET les prérequis
void unlock_tech(Game* game, TechTree* tree, int tech_id);    // Paie la science et passe is_unlocked à 1

// Afficher le menu interactif
void show_technology_menu(Game* game);

// Vérifie si un bâtiment (ex: 'G' pour Grenier) est débloqué
int is_building_unlocked(TechTree* tree, char building_type);

// Vérifie si une unité (ex: 'c' pour Colon) est débloquée
int is_unit_unlocked(TechTree* tree, char unit_type);
#endif
