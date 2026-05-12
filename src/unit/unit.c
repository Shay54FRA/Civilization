#include "unit.h"
#include "../game/game.h"
#include "../tile/tile.h"
#include <stdio.h>
#include <stdlib.h>

// --- Création & Destruction ---

Unit* create_unit(char type, Position pos) {
    Unit* u = malloc(sizeof(Unit));
    if (u == NULL) return NULL;

    u->type = type;
    u->pos = malloc(sizeof(Position));
    if (u->pos != NULL) {
        u->pos->x = pos.x;
        u->pos->y = pos.y;
    }

    // Statistiques
    if (type == 'c') { // Colon
        u->max_pv = 1;  u->pv = 1;
        u->atk = 0;     u->def = 0;
        u->max_pm = 2;  u->pm = 2;
        u->cost_per_turn = 0;
    } 
    else if (type == 'g') { // Guerrier
        u->max_pv = 15; u->pv = 15;
        u->atk = 3;     u->def = 2;
        u->max_pm = 3;  u->pm = 3;
        u->cost_per_turn = 1;
    }
    // Tu pourras rajouter d'autres unités ici plus tard
    
    return u;
}

void destroy_unit(Unit* unit) {
    if (unit != NULL) {
        if (unit->pos != NULL) free(unit->pos);
        free(unit);
    }
}

// --- Getters ---

int get_atk(Unit* unit) { return unit ? unit->atk : 0; }
int get_pv(Unit* unit) { return unit ? unit->pv : 0; }
int get_def(Unit* unit) { return unit ? unit->def : 0; }
int get_pm(Unit* unit) { return unit ? unit->pm : 0; }
int get_cost_per_turn(Unit* unit) { return unit ? unit->cost_per_turn : 0; }
char get_unit_type(Unit* unit) { return unit ? unit->type : '\0'; }

Position get_unit_pos(Unit* unit) { 
    Position empty = {-1, -1};
    return (unit && unit->pos) ? *(unit->pos) : empty; 
}

Unit* get_unit(UnitList* lst) {
    return lst ? lst->data : NULL;
}

UnitList* get_unit_next(UnitList* lst) {
    // Problème de logique résolu : on renvoie juste le pointeur suivant s'il existe !
    return lst ? lst->next : NULL;
}

// --- NOUVELLES FONCTIONS DE MÉCANIQUE ---

// Fait apparaître une unité sur la carte à la fin d'un projet de ville
void spawn_unit_from_project(Game* game, City* city) {
    if (!city || !city->project) return;
    
    char p_type = city->project->type;
    
    // Vérifier que c'est bien une unité ('c' ou 'g')
    if (p_type == 'c' || p_type == 'g') {
        Unit* new_unit = create_unit(p_type, city->project->pos);
        
        // 1. Ajouter à la liste du joueur
        UnitList* node = malloc(sizeof(UnitList));
        node->data = new_unit;
        node->next = game->unitList;
        game->unitList = node; // Insère en tête de liste

        // 2. Placer physiquement sur la Tile de la carte
        Tile* tile = get_tile(game->map, city->project->pos);
        if (tile) tile->unit = new_unit;

        printf("-> Une unite '%c' a ete formee a la position (%d, %d) !\n", p_type, new_unit->pos->x, new_unit->pos->y);
    }
}

// Helper interne pour avoir le coût en PM selon le biome
static int get_terrain_cost(char biome) {
    if (biome == 'E') return 999; // Eau infranchissable
    if (biome == 'F' || biome == 'M') return 2; // Forêt & Montagne
    return 1; // Plaine, Désert, Toundra (valeur par défaut)
}

// Gère le combat automatique si on se déplace sur une case ennemie
void resolve_combat(Game* game, Unit* attacker, Unit* target, Tile* target_tile) {
    // Si l'attaquant est inoffensif (ex: Colon)
    if (attacker->atk == 0) return;

    // Calculs combat automatique 
    int dmg_to_target = attacker->atk - target->def;
    if (dmg_to_target < 1) dmg_to_target = 1;

    int dmg_to_attacker = target->atk - attacker->def;
    if (dmg_to_attacker < 0) dmg_to_attacker = 0;

    target->pv -= dmg_to_target;
    attacker->pv -= dmg_to_attacker;

    if (target->pv <= 0) {
        printf("*** L'unite cible '%c' a ete detruite ! ***\n", target->type);
        target_tile->unit = NULL;
        // Note: Ici, il faudra aussi retirer `target` de sa UnitList respective (Player ou Barbare)
    }
    
    if (attacker->pv <= 0) {
        printf("*** L'unite attaquante '%c' a ete detruite en retour ! ***\n", attacker->type);
        Tile* atk_tile = get_tile(game->map, *(attacker->pos));
        if (atk_tile) atk_tile->unit = NULL;
    }
}

// Tente de déplacer une unité d'une case (renvoie true si succès)
bool move_unit_step(Game* game, Unit* unit, Position dest) {
    if (!unit || unit->pm <= 0) return false;

    Tile* dest_tile = get_tile(game->map, dest);
    if (!dest_tile) return false;

    // 1. L'Eau est infranchissable
    if (dest_tile->biome == 'E') {
        printf("Deplacement impossible : tu ne peux pas marcher sur l'Eau !\n");
        return false;
    }

    // 2. Vérification des PM
    int cost = get_terrain_cost(dest_tile->biome);
    if (unit->pm < cost) {
        printf("Deplacement impossible : pas assez de PM (requis: %d, actuels: %d).\n", cost, unit->pm);
        return false;
    }

    // 3. Vérification de la case de destination
    if (dest_tile->unit != NULL) {
        // TODO: Ajouter une vérification "est-ce un allié ou un ennemi ?"
        // Pour l'instant, on suppose que s'il y a une unité, on combat.
        resolve_combat(game, unit, dest_tile->unit, dest_tile);
        
        unit->pm -= cost; 
        return false;   
    }

    // 4. Déplacement validé : on bouge !
    unit->pm -= cost;

    // Vider l'ancienne case
    Tile* current_tile = get_tile(game->map, *(unit->pos));
    if (current_tile) current_tile->unit = NULL;

    // Occuper la nouvelle case
    dest_tile->unit = unit;
    unit->pos->x = dest.x;
    unit->pos->y = dest.y;

    return true;
}

// A appeler à chaque nouveau tour dans ta boucle de jeu principale
void reset_all_pm(UnitList* lst) {
    UnitList* current = lst;
    while (current != NULL) {
        if (current->data != NULL) {
            current->data->pm = current->data->max_pm;
        }
        current = current->next;
    }
}