#ifndef TECHNOLOGY_H
#define TECHNOLOGY_H

// On inclut game.h pour pouvoir modifier la science plus tard si besoin
typedef struct _Game Game; 

/* 
 * 1. La "plus petite structure" pour gérer les bonus.
 */
typedef struct _TechBonus{
    // Les déblocages
    char unlocked_buildings[5];
    int unlocked_buildings_count;
    char unlocked_units[5];
    int unlocked_units_count;

    // Bonus passifs globaux (pourcentages)
    int bonus_food_percent;       // ex: 10 pour l'Agriculture (+10%)
    int bonus_production_percent; // ex: 10 pour l'Artisanat (+10%)
    int bonus_science_percent;    // ex: 10 pour l'Écriture (+10%)
    int bonus_gold_percent;       // ex: 10 pour le Commerce (+10%)

    // Bonus passifs spécifiques
    int bonus_food_forest;        // ex: 1 pour la Chasse (+1 Nourriture sur Forêt)
    int bonus_pm_units;           // ex: 1 pour l'Équitation (+1 Point de Mouvement)
} TechBonus;

/*
 * 2. Le noeud principal : la Technologie
 */
typedef struct _Technology{
    int id;
    char name[50];
    int science_cost;
    
    // --- CE QUI EST IMPOSÉ PAR LE CDC : Prérequis multiples ---
    int prerequisites[3];        // Tableau des IDs requis (3 maximum pour commencer au pire on peut modifier)
    int num_prerequisites;       // Combien de prérequis cette techno demande
    
    int is_unlocked;
    TechBonus bonus;
} Technology;

/*
 * 3. L'arbre complet qui regroupe tout
 */
typedef struct _TechTree {
    int num_technologies;
    Technology* technologies;   // Tableau dynamique contenant toutes les technos du jeu
    
    // --- Tableaux booléens pour informer et stocker dans le jeu une technologie découverte ---
    int unlocked_buildings_global[256];
    int unlocked_units_global[256];

    /* Résumé de tous les bonus acquis */
    int bonus_food_percent;
    int bonus_prod_percent;
    int bonus_gold_percent;
    int bonus_science_percent;
    
    int bonus_food_forest;
    int bonus_pm_units;
} TechTree;

// --- Fonctions principales ---

// Créer et détruire l'arbre
TechTree* create_tech_tree(void);
void destroy_tech_tree(TechTree* tree);

// Fonctions de logique de jeu
int can_research_tech(Game* game, TechTree* tree, int tech_id);
void set_active_research(Game* game, int tech_id);
void update_research(Game* game);

// Afficher le menu interactif
void show_technology_menu(Game* game);

// Vérifie si un bâtiment (ex: 'G' pour Grenier) est débloqué
int is_building_unlocked(TechTree* tree, char building_type);

// Vérifie si une unité (ex: 'c' pour Colon) est débloquée
int is_unit_unlocked(TechTree* tree, char unit_type);
#endif
