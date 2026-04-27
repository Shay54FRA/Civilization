#include "map.h"
#include "building.h"
#include <stdlib.h>
#include <stdio.h>

Building* create_building(char type, Position pos){
    Building* rep = malloc(sizeof(Building));
    rep->type = type;
    rep->pos = pos;
    return rep;
}
void destroy_building(Building* build) {
    if (build != NULL) {
        free(build);
    }
}

char get_building_type(Building* build) {
    if (build != NULL) {
        return build->type;
    }
    return '\0';
}

Position get_building_pos(Building* build) {
    if (build != NULL) {
        return build->pos;
    }
    Position error_pos = {-1,-1};
    return error_pos;
}

char* get_building_name(Building* build) { //2 options, faire un dico ou bien des if pas très beaux
    if (build == NULL) {
        return NULL;
    }
    char test = get_building_type(build);
    if (test == 'G') return "Grenier";
    if (test == 'M') return "Marché";
    if (test == 'A') return "Atelier";
    if (test == 'B') return "Bibliothèque";
    if (test == 'C') return "Caserne";
    if (test == 'R') return "Muraille";
    return NULL;
}

void print_building(Building* build) {
    if (build == NULL) {
        printf("NULL");
    }
    else {
        printf("%s : x = %d | y = %d\n", get_building_name(build), build->pos.x, build->pos.y);
    }
}

int get_building_entretien_cost(Building* build) {
    if (build == NULL) {
        return -1; //A traiter plus tard dans les bonus 
    }
    char type = get_building_type(build);
    if (type == 'G' || type == 'M' || type == 'A' || type == 'B') {
        return 1;
    }
    if (type == 'C' || type == 'R') {
        return 2;
    }
    return -2; // erreur idem -1
}

BuildList* create_buildlist(Building* build) { //Une ville commence tjrs avec un grenier 
    BuildList* rep = malloc(sizeof(BuildList));
    rep->data = build;
    rep->next = NULL;
    return rep;
}
void destroy_buildlist(BuildList* lst) {
    if (lst != NULL) {
        if (get_buildlist_next(lst) != NULL) {
            destroy_buildlist(get_buildlist_next(lst));
        }
        destroy_building(get_buildlist_data(lst));
        free(lst);
    }
}

void append_buildlist(BuildList* lst, Building* build) {
    BuildList* to_add = create_buildlist(build);
    BuildList* to_check = lst;
    if (to_check != NULL) {
        while(get_buildlist_next(to_check) != NULL) {
            to_check = get_buildlist_next(to_check);
        }
        to_check->next = to_add;
    }
}

Building* get_buildlist_data(BuildList* lst){
    if (lst != NULL) {
        return lst->data;
    }
    return NULL;
}
BuildList* get_buildlist_next(BuildList* lst){
    if (lst != NULL) {
        return lst->next;
    }
    return NULL;
}

void print_buildlist(BuildList* lst) {
    BuildList* to_check = lst;
    if (to_check != NULL) {
        printf("Batiments : \n");
        while (to_check != NULL) {
            print_building(get_buildlist_data(to_check));
            to_check = get_buildlist_next(to_check);
        }
    }
}
