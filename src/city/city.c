#include "city.h"
#include "../building/building.h"
#include "../map/map.h"
#include <stdlib.h>
#include <stdio.h>
#include "../game/game.h"
#include "../unit/unit.h"
#include "../tile/tile.h"
#include <stdbool.h>
#include <math.h>

City* create_city(Position pos) {
    Building* build = create_building('G');
    City* city = malloc(sizeof(City));
    if (city == NULL || build == NULL) return NULL;
    city->food = 0;
    city->pos = pos;
    city->population = 1;
    city->production = 0;
    city->strength = 20;
    city->basements_number = 1;
    city->walls_number = 0;
    city->damage = 0; //Au lieu de stocker les pv actuels on stocks les dégâts reçus
    city->project = NULL;
    city->new_ressources = create_tuple_ressources();
    city->buildings = create_buildlist(build);
    city->can_produce_unit = false;
    city->has_taken_damage = false;
    return city;
}

void destroy_city(City* city) {
    if (city == NULL) return;
    destroy_project(city);
    destroy_tuple_ressources(city->new_ressources);
    destroy_buildlist(city->buildings);
    free(city);
}

void kill_city(Game* game, City* city) {
    if (game == NULL || city == NULL) return;
    CityList* to_check = game->cityList;
    CityList* previous = NULL;
    while (to_check != NULL) {
        if (to_check->city == city) { // On a trouvé la ville en question
            Tile* tile = get_tile(game, city->pos);
            if (tile != NULL) {
                tile->city_on = false;
                if (previous == NULL) {
                    game->cityList = to_check->next;
                } else {
                    previous->next = to_check->next;
                }
                destroy_city(city);
                free(to_check);
                break;
            }
        }
        previous = to_check;
        to_check = to_check->next;
    }
}

int get_population(City* city){
    if (city != NULL) {
        return city->population;
    }
    return -1;
}

int get_city_pv(City* city){
    if (city != NULL) {
        return (MAX_HP - city->damage);
    }
    return -1;
}

int get_food(City* city){
    if (city != NULL) {
        return city->food;
    }
    return -1;
}

int get_production(City* city){
    if (city != NULL) {
        return city->production;
    }
    return -1;
}

Project* get_project(City* city){
    if (city != NULL) {
        return city->project;
    }
    return NULL;
}

BuildList* get_buildings_list(City* city){
    if (city != NULL) {
        return city->buildings;
    }
    return NULL;
}

bool croissance_check(City* city) {
    if (CROISSANCE_NEED <= get_food(city)) {
        city->population += 1;
        city->food = 0;
        return true;
    }
    return false;
}

void end_city(CityList* citylist, City* city) {
    /* A compléter */
    return;
}

int get_new_food(City* city) {
    if (city == NULL) return -1;
    if (city->new_ressources == NULL) return -1;
    return city->new_ressources->ressource1;
}
int get_new_prod(City* city) {
    if (city == NULL) return -1;
    if (city->new_ressources == NULL) return -1;
    return city->new_ressources->ressource2;
}

City* get_city_on_tile(CityList* liste_city, Tile* tile) {
    if (tile == NULL) return NULL;
    if (!tile->city_on) return NULL;
    CityList* to_check = liste_city;
    Position pos = tile->pos;
    while (to_check != NULL) {
        City* city = to_check->city;
        Position pos_to_check = city->pos;
        if (pos_to_check.x == pos.x && pos_to_check.y == pos.y) {
            return city;
        }
        to_check = to_check->next;
    }
    return NULL;
}

int get_distance_to_city(City* city, Position pos) {
    if (city != NULL) {
        int dist = get_distance(city->pos, pos);
        return dist;
    }
    return -1;
        /*if (city->buildings != NULL) {
            if (city->buildings->data != NULL) {
                rep = city->buildings->data->pos;
                int min = get_distance(rep, pos); // Par rapport au premier batîment
                int dist;
                BuildList* to_check = get_buildlist_next(city->buildings);
                while (to_check != NULL) {
                    Building* bat = get_buildlist_data(to_check);
                    dist = get_distance(bat->pos, pos);
                    if (min > dist) {
                        rep = bat->pos;
                        min = dist;
                    }
                    to_check = to_check->next;
                }
                return rep;
            }
        }
    }*/
}

bool start_project(City* city, char type, Position pos) {
    if (city->project == NULL) {
        Project* project = malloc(sizeof(Project));
        project->pos = pos;
        project->type = type;
        project->production_cost = get_cost(type);
        city->project = project;
        return true;
    }
    return false;
}

bool end_project(Game* game, City* city) {
    if (city != NULL) {
        if (city->project != NULL) {
            city->project->production_cost -= get_production(city);
            if (city->project->production_cost <= 0) {
                if (city->project->type == 'c' || city->project->type == 'g') {
                    spawn_unit_from_project(game, city);
                }
                else {
                    /* A COMPLETER PLUS TARD */
                }

                destroy_project(city);
                return true;
            }
        }
    }
    return false;
}

void destroy_project(City* city) {
    if (city != NULL) {
        if (city->project != NULL) {
            free(get_project(city));
            city->project = NULL;
        }
    }
}

char get_project_type(City* city) {
    if (city != NULL ) {
        if (get_project(city) != NULL) {
            return city->project->type;
        }
    }
    return '\0';
}

char* get_project_name(City* city){
    if (city != NULL) {
        if (get_project(city) != NULL) {
            return (get_name(city->project->type));
        }
    }
    return NULL;
}

int get_production_left(City* city) {
    if (city != NULL) {
        if (get_project(city) != NULL) return get_project(city)->production_cost;
    }
    return -1;
}

CityList* create_city_list(Game* game, City* city) {
    CityList* city_list = malloc(sizeof(CityList));
    if (city_list != NULL) {
        city_list->city = city;
        city_list->next = game->cityList;
    }
    return city_list;
}

void destroy_city_list(CityList* city_list) {
    if (city_list != NULL) {
        destroy_city(city_list->city);
        destroy_city_list(city_list->next);
        free(city_list);
    }
}

City* get_city(CityList* lst){
    if (lst != NULL) return lst->city;
    return NULL;
}

CityList* get_next_city(CityList* lst){
    if (lst != NULL) return lst->next;
    return NULL;
}

int max(int a, int b) {
    if (a >= b) return a;
    return b;
}

void heal_city(City* city) {
    if (city->has_taken_damage) {
        city->has_taken_damage = false; //On réinitialise pour le prochain tour
    } else {
        city->damage = max(0, city->damage - (MAX_HP/2));
    }
}

void heal_cities(CityList* city_list) {
    CityList* to_check = city_list;
    while (to_check != NULL) {
        if (to_check->city != NULL) {
            heal_city(to_check->city);
        }
        to_check = to_check->next;
    }
}

void update_food(CityList* city_list) {
    CityList* to_check = city_list;
    while (to_check != NULL) {
        if (to_check->city != NULL) {
            City* city = to_check->city;
            city->food -= FOOD_NEEDS;
        }
        to_check = to_check->next;
    }
}

bool check_famine(Game* game, City* city) {
    if (city == NULL) return false;
    if (city->food < 0) {
        city->population -= 1;
        if (city->population <= 0) {
            kill_city(game, city);
        }
        return true;
    }
    return false;
}

void update_croissance(Game* game) {
    CityList* to_check = game->cityList;
    while (to_check != NULL) {
        if (to_check->city != NULL) {
            City* city = to_check->city;
            croissance_check(city);
            check_famine(game, city);
        }
        to_check = to_check->next;
    }
}
