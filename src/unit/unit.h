#ifndef UNIT
#define UNIT

#include "../map/map.h" // Nécessaire pour connaître Position et Tile
#include "../city/city.h" // Nécessaire pour lier à la ville
#include <stdbool.h>

typedef struct _Game Game; 

typedef struct _Unit {
    char type;
    int atk;
    int def;
    int cost_per_turn; // Entretien
    int pv;
    int max_pv;        
    int pm;
    int max_pm;       
    Position* pos;
} Unit;

typedef struct _UnitList { // Liste chaînée pour regrouper toutes les unités de la partie
    Unit* data;
    struct _UnitList* next;
} UnitList;

// --- Création & Destruction ---
// Mise à jour de la signature pour prendre le type et la position au moment de la création
Unit* create_unit(char type, Position pos);
void destroy_unit(Unit* unit);

// --- Getters existants ---
int get_atk(Unit* unit);
int get_pv(Unit* unit);
int get_def(Unit* unit);
int get_pm(Unit* unit);
int get_cost_per_turn(Unit* unit);
char get_unit_type(Unit* unit);
Position get_unit_pos(Unit* unit);

Unit* get_unit(UnitList* lst);
UnitList* get_unit_next(UnitList* lst); 

// --- Fonctions de Mécanique (Mouvement, Combat, Spawning) ---
void spawn_unit_from_project(Game* game, City* city); 
bool move_unit_step(Game* game, Unit* unit, Position dest); // Gère 1 case de déplacement
void resolve_combat(Game* game, Unit* attacker, Unit* target, Tile* target_tile);
void reset_all_pm(UnitList* lst); // A appeler au début de chaque tour

#endif
