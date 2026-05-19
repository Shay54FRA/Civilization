#ifndef BARBARIAN
#define BARBARIAN

#include "../map/map.h"
#include <stdbool.h>

#define MAX_BARBS (3*game->configuration->b)

typedef struct _Game Game;
typedef struct _Position Position;

typedef struct _Barbarian {
    int pv;
    int pm;
    int max_pm;
    int atk;
    int def;
    Position* pos;
} Barbarian;

typedef struct _BarbarianList {
    Barbarian* data;
    struct _BarbarianList* next;
} BarbarianList;

Barbarian* create_barbarian(Position pos);
void destroy_barbarian(Barbarian* barbarian);
BarbarianList* create_barb_list(Barbarian* barb, BarbarianList* next);
void destroy_barb_list(BarbarianList* barb_list);

int get_barb_pv(Barbarian* barb);
int get_barb_pm(Barbarian* barb);
Position* get_barb_pos(Barbarian* barb);

//La position de la ville ou l'unité la plus proche
Position get_nearest_target(Game* game, Barbarian* barb); 
/* Pour les villes on prendra le min de la distance avec chacun des batiments de la ville */

//Calculer la direction nécéssaire pour se rapprocher et l'applique
void move_barbarian(Game* game, Barbarian* barb, Position pos_cible); 

//Attaque du barbare
int barbarian_attack(Game* game, Barbarian* barb, Position pos);

//Lorsqu'il meurt au combat
void kill_barbarian(Game* game, Barbarian* barb);

//Deplacer tous les barbares
void move_all_barbarians(Game* game);

void reset_all_barbs_pm(BarbarianList* barb_list);

//Si c'est un guerrier il peut détruire un camp s'il est à la même position, ce qui rapporte une quantité d'or à calculer en fonction du nombre
//de camps restants sur la carte et renvoyer la quantité d'or remportée
int destroy_camp(Game* game, Position pos);

//Générer tous les barbares lors de ce tour
void spawn_all_barbarians(Game* game);


#endif
