#ifndef BARBARIAN
#define BARBARIAN

#include "../map/map.h"
#include <stdbool.h>

typedef struct _Game Game;

typedef struct _Barbarian {
    int pv;
    int pm;
    Position* pos;
} Barbarian;

typedef struct _BarbarianList {
    Barbarian* data;
    struct _Barbarian* next;
} BarbarianList;

int get_barb_pv(Barbarian barb);
int get_barb_pm(Barbarian barb);
int get_barb_pos(Barbarian barb);

//void* pour renvoyer au choix Unit* ou City*
void* get_nearest_target(Game* game, Barbarian* barb); 
/* Pour les villes on prendra le min de la distance avec chacun des batiments de la ville */

//Calculer la direction nécéssaire pour se rapprocher et l'applique
void move_barbarian(Game* game, Barbarian* barb, void* target); 

//Attaque du barbare
void barbarian_attack(Barbarian* barb, void* target);

//Deplacer tous les barbares
void move_all_barbarians(BarbarianList* barbarian_list);

typedef struct _Camp {
    Position* pos;
    bool occupied; //Dit si la case est occupé (pour la génération de troupe)
} Camp;

typedef struct _CampList {
    Camp* data;
    struct _CampList* next;
} CampList;

Camp* get_camplist_data(CampList* lst);
CampList* get_camplist_next(CampList* lst);

//Si c'est un guerrier il peut détruire un camp s'il est à la même position, ce qui rapporte une quantité d'or à calculer en fonction du nombre
//de camps restants sur la carte
void destroy_camp(Game* game, Camp* camp);

//Générer tous les barbares lors de ce tour
void spawn_all_barbarians(CampList* camp_list);


#endif
