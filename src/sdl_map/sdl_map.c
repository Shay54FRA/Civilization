#include "sdl_map.h"
#include "../map/map.h"
#include "../tile/tile.h"
#include "../game/game.h"
#include "../city/city.h"
#include "../building/building.h"
#include "../unit/unit.h"
#include <SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>



// Fonction pour trouver la ville correspondente en fct de la position sur la carte
City* find_city_at_position(Game* game, Position pos) {
    CityList* current = game->cityList; 

    while (current != NULL) {

        City * city = get_city(current);

        if (city != NULL) {
            BuildList* blist = get_buildings_list(city);

            if (blist != NULL) {
                Building* b = get_buildlist_data(blist);

                if (b != NULL) {
                    // On récupère la position via le getter officiel
                    Position city_pos = get_building_pos(b);

                    if (city_pos.x == pos.x && city_pos.y == pos.y) {
                        return city;
                    }
                }
            }
        }
        current = get_next_city(current);
    }
    return NULL;
}


// Détecte si un bâtiment est construit sur une case précise
char get_building_type_at(Game* game, Position pos) {
    CityList* c_list = game->cityList;
    while (c_list != NULL) {

        City* city = get_city(c_list);
        BuildList* b_list = get_buildings_list(city);

        while (b_list != NULL) {
            Building* b = get_buildlist_data(b_list);

            if (b != NULL) {
                Position b_pos = get_building_pos(b);

                if (b_pos.x == pos.x && b_pos.y == pos.y) {
                    return get_building_type(b); // Renvoie 'G', 'M', etc.
                }
            }
            b_list = get_buildlist_next(b_list);
        }
        c_list = get_next_city(c_list);
    }
    return '\0'; // Aucun bâtiment ici
}


// Détecte si un projet de bâtiment en construction cible cette case
char get_project_building_at(Game* game, Position pos) {
    CityList* c_list = game->cityList;
    while (c_list != NULL) {
        Project* proj = c_list->city->project;
        // Si un projet existe et que c'est un bâtiment (pas une unité 'c' ou 'g')
        if (proj && proj->pos.x == pos.x && proj->pos.y == pos.y) {
            if (proj->type != 'c' && proj->type != 'g') {
                return proj->type;
            }
        }
        c_list = c_list->next;
    }
    return '\0';
}

SDL_Color get_biome_color(char biome) {
    switch (biome) {
        case 'E': return (SDL_Color){0, 105, 148, 255};   // Eau (Bleu)
        case 'P': return (SDL_Color){34, 139, 34, 255};   // Plaine (Vert)
        case 'F': return (SDL_Color){0, 100, 0, 255};     // Forêt (Vert foncé)
        case 'M': return (SDL_Color){128, 128, 128, 255}; // Montagne (Gris)
        case 'D': return (SDL_Color){194, 178, 128, 255}; // Désert (Sable)
        case 'T': return (SDL_Color){210, 210, 210, 255}; // Toundra (Gris clair)
        default:  return (SDL_Color){255, 255, 255, 255}; // Cas de base
    }
}

const char* get_biome_name(char biome) {
    switch (biome) {
        case 'E': return "Eau";
        case 'P': return "Plaine";
        case 'F': return "Foret";
        case 'M': return "Montagne";
        case 'D': return "Desert";
        case 'T': return "Toundra";
        default:  return "Inconnu";
    }
}

void draw_hexagones(SDL_Renderer* renderer, int x, int y, int R, SDL_Color color) {
    int h = (int)(R * 0.866f); 

    const Sint16 vx[6] = {x, x + h, x + h, x, x - h, x - h};
    const Sint16 vy[6] = {y - R, y - R/2, y + R/2, y + R, y + R/2, y - R/2};

    // appartient à la bibliothèque sdl2_gfxs
    filledPolygonRGBA(renderer, vx, vy, 6, 
                      color.r, color.g, color.b, color.a); 
    
    // + petit contour en noir ici
    polygonRGBA(renderer, vx, vy, 6, 0, 0, 0, 255); 
}




void draw_map_sdl(SDL_Renderer* renderer, Game* game, int R, int h, Position position_actuelle, int cameraX, int cameraY, 
                SDL_Texture* tex_ville, SDL_Texture* tex_ville_mur, SDL_Texture* tex_guerrier, SDL_Texture* tex_colon, 
                SDL_Texture* tex_camp, SDL_Texture* tex_barbare, SDL_Texture* tex_bat_const){

    // On regarde si la case actuellement sélectionnée contient une ville
    City* selected_city = NULL;
    if (position_actuelle.x != -1) {
        selected_city = find_city_at_position(game, position_actuelle);
    }
    
    for (int i = 0; i < game->map->height; i++) {
        for (int j = 0; j < game->map->length; j++) {
            Tile* tuile = game->map->map[i][j];


            //Calcule de la position puis on soustrait la caméra
            int x = j * (2 * h) + ((i % 2) * h) - cameraX + (1280 / 2);
            int y = i * (1.5f * R) - cameraY + (720 / 2);

            // On récupère la SDL_Color directement pour la mettre dans l'hexagone
            SDL_Color color = get_biome_color(tuile->biome);
            draw_hexagones(renderer, x, y, R, color);

            //VISUALISATION ZONE D'EXPLOITATION
            if (selected_city != NULL) {
                BuildList* blist = get_buildings_list(selected_city);

                if (blist != NULL) {
                    Building* b = get_buildlist_data(blist);

                    if (b != NULL) {
                        Position city_center = get_building_pos(b);

                        if (get_distance(city_center, tuile->pos) <= 2) { 
                            filledPolygonRGBA(renderer, 
                                (Sint16[]){x, x + h, x + h, x, x - h, x - h},
                                (Sint16[]){y - R, y - R/2, y + R/2, y + R, y + R/2, y - R/2},
                                6, 255, 255, 0, 40);
                        }
                    }
                }
            }

            // surbrillance de la position où l'on est + verif qu'on est pas en dehors de la carte
            if (position_actuelle.x != -1 && tuile->pos.x == position_actuelle.x && tuile->pos.y == position_actuelle.y) {

                // On dessine un contour blanc épais (3 hexagones de tailles très proches)
                for(int sw = 0; sw < 3; sw++) {

                    polygonRGBA(renderer, 
                        (Sint16[]){x, x + h - sw, x + h - sw, x, x - h + sw, x - h + sw},
                        (Sint16[]){y - R + sw, y - R/2 + sw/2, y + R/2 - sw/2, y + R - sw, y + R/2 - sw/2, y - R/2 + sw/2},
                        6, 255, 255, 255, 255);
                }
            }

            // Zone de dessin du sprite (centrée sur l'hexagone)
            // Si R = 60, un sprite de 48x48 ou 54x54 est parfait et bien centré
            int sprite_size = 110; 
            SDL_Rect dstRect = { x - (sprite_size / 2), y - (sprite_size / 2), sprite_size, sprite_size };

            // Dessin du Camp Barbare
            if (tuile->camp_on && tex_camp) {
                SDL_RenderCopy(renderer, tex_camp, NULL, &dstRect);
            }

            // Dessin du Centre Ville Principal
            if (tuile->city_on && tex_ville) {
                SDL_RenderCopy(renderer, tex_ville, NULL, &dstRect);
            }
            
            // Dessin des Bâtiments Annexes (Murailles isolées ou Bâtiments terminés/en cours)
            else {
                char b_type = get_building_type_at(game, tuile->pos);
                char p_type = get_project_building_at(game, tuile->pos);

                if (b_type == 'R') { 
                    // C'est une muraille annexe ! Elle utilise le sprite ville comme demandé
                    if (tex_ville) SDL_RenderCopy(renderer, tex_ville, NULL, &dstRect);
                } 
                else if (b_type != '\0' || p_type != '\0') {
                    // C'est un autre bâtiment terminé OU un chantier en cours -> Sprite chantier !
                    if (tex_bat_const) SDL_RenderCopy(renderer, tex_bat_const, NULL, &dstRect);
                }
            }

            // Dessin de l'unité
            if (tuile->unit != NULL) {
                SDL_Texture* tex_unit = NULL;
                char type = tuile->unit->type;
                if (type == 'g' || type == 'G') tex_unit = tex_guerrier;
                else if (type == 'c' || type == 'C') tex_unit = tex_colon;
                else if (type == 'b' || type == 'B') tex_unit = tex_barbare;
                
                if (tex_unit) SDL_RenderCopy(renderer, tex_unit, NULL, &dstRect);
            }
            
        }   
    }
}


Position position_hexagone(int mx, int my, int R, int h, int cameraX, int cameraY, Game* game) {
    Position p = {-1, -1}; // coordonnées initiales en dehors du tableau

    //On va utiliser la méthode de la distance carrée
    //On va regarder la case dont le centre de dessin est le plus proche de ma souris
    //Et pour éviter une fonction racine carrée lourde et lente, on utilise la distance au carré

    long min_dist_carree = 99999999; //on commence avec une distance infinie

    for (int i = 0; i < game->map->height; i++) {
        for (int j = 0; j < game->map->length; j++) {
            
            // On calcule le centre exact en pixels de cet hexagone (la même formule que pour le dessin de l'hexagone)
            int x = j * (2 * h) + ((i % 2) * h) - cameraX + (1280 / 2);
            int y = i * (1.5f * R) - cameraY + (720 / 2);

            // Calcul de la distance entre la souris (mx, my) et le centre de l'hexagone (x, y)
            long dx = mx - x;
            long dy = my - y;
            long dist_carree = (dx * dx) + (dy * dy); // Pas besoin de racine carrée !

            // Si ce centre est le plus proche qu'on ait trouvé jusqu'ici
            if (dist_carree < min_dist_carree) {
                min_dist_carree = dist_carree;
                p.x = j;
                p.y = i;
            }
        }
    }

    // Sécurité optionnelle : Si on clique trop loin de l'hexagone le plus proche (dans le grand vide noir dehors)
    // Le rayon au carré d'une case est R * R. Si on est plus loin, on considère qu'on a cliqué dans le vide.
    if (min_dist_carree > (R * R)) {
        p.x = -1;
        p.y = -1;
    }

    return p;
}