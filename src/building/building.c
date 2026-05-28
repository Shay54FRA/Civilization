#include "../map/map.h"
#include "building.h"
#include "../game/game.h"
#include <stdlib.h>
#include <stdio.h>

Building* create_building(char type){
    Building* rep = malloc(sizeof(Building));
    rep->type = type;
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

/*Position get_building_pos(Building* build) {
    if (build != NULL) {
        return build->pos;
    }
    Position error_pos = {-1,-1};
    return error_pos;
}*/

void print_building(Building* build) {
    if (build == NULL) {
        printf("NULL");
    }
    else {
        printf("%s\n", get_name(get_building_type(build)));
    }
}

BuildList* create_buildlist(Building* build, BuildList* next) { //Une ville commence tjrs avec un grenier 
    BuildList* rep = malloc(sizeof(BuildList));
    rep->data = build;
    rep->next = next;
    return rep;
}

void destroy_buildlist(BuildList* lst) {
    if (lst != NULL) {
        destroy_buildlist(get_buildlist_next(lst));
        destroy_building(get_buildlist_data(lst));
        free(lst);
    }
}

void append_buildlist(BuildList* lst, Building* build) {
    if (build == NULL) return;
    if (lst == NULL) {
        lst = create_buildlist(build, NULL); 
        return;
    }
    BuildList* to_check = lst;
    BuildList* old_lst = NULL;
    while(to_check != NULL) {
        old_lst = to_check;
        to_check = to_check->next;
    }
    old_lst->next = create_buildlist(build, NULL);
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

bool buildlist_contains(BuildList* lst, char type) {
    BuildList* to_check = lst;

    while (to_check != NULL) {
        Building* build = get_buildlist_data(to_check);

        if (build != NULL && get_building_type(build) == type) {
            return true;
        }

        to_check = get_buildlist_next(to_check);
    }

    return false;
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
