#include "sdl_panneaux.h"
#include "../sdl_map/sdl_map.h" 
#include "../game/game.h"
#include "../tile/tile.h"
#include "../unit/unit.h"
#include "../building/building.h"
#include "../configuration/configuration.h"
#include "../barbarian/barbarian.h"
#include "../technology/technology.h"
#include <SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Convertit le résultat d’un déplacement en texte
void get_move_message(MoveResult result, char* buffer, size_t size) {
    switch (result) {
        case MOVE_OK:
            snprintf(buffer, size, "Deplacement effectue !"); break;

        case MOVE_NO_UNIT:
            snprintf(buffer, size, "Aucune unite selectionnee"); break;

        case MOVE_NO_PM:
            snprintf(buffer, size, "Cette unite n'a plus de PM"); break;

        case MOVE_INVALID_TILE:
            snprintf(buffer, size, "Case invalide"); break;

        case MOVE_WATER:
            snprintf(buffer, size, "Impossible : eau infranchissable"); break;

        case MOVE_NOT_ADJACENT:
            snprintf(buffer, size, "Il faut se deplacer case par case"); break;

        case MOVE_NOT_ENOUGH_PM:
            snprintf(buffer, size, "Pas assez de PM"); break;

        case MOVE_ALLY_OCCUPIED:
            snprintf(buffer, size, "Case occupee par une unite alliee"); break;

        default:
            snprintf(buffer, size, "Action ou deplacement impossible"); break;
    }
}



// Panneau Message d'action (Flotte en haut au centre)
void draw_panneau_message_action(SDL_Renderer* renderer, const char* message, int screenW) {
    int w = 500;
    int x1 = (screenW - w) / 2;
    boxRGBA(renderer, x1, 10, x1 + w, 45, 0, 0, 0, 180);
    rectangleRGBA(renderer, x1, 10, x1 + w, 45, 200, 200, 200, 255);
    stringRGBA(renderer, x1 + 20, 22, message, 255, 255, 100, 255);
}


void draw_panneau_global(SDL_Renderer* renderer, Game* game) {
    // Boîte noire semi-transparent (X=10, Y=10, Largeur=310, Hauteur=155)
    boxRGBA(renderer, 10, 10, 310, 155, 0, 0, 0, 200);
    
    // Bordure fine grise pour faire stylé
    rectangleRGBA(renderer, 10, 10, 310, 155, 150, 150, 150, 255);

    char txt_tour[50];
    char txt_ressources[100];

    sprintf(txt_tour, "TOUR : %d / %d", game->active_turn, game->configuration->t);
    sprintf(txt_ressources, "OR : %d  |  SCIENCE : %d", game->gold, game->science);

    // Affichage des textes
    stringRGBA(renderer, 25, 25, txt_tour, 255, 255, 255, 255);
    stringRGBA(renderer, 25, 50, txt_ressources, 255, 215, 0, 255); // Écrit en couleur dorée

    // Dessin du Bouton Technologies (Bleu)
    boxRGBA(renderer, 20, 75, 150, 105, 30, 80, 150, 255); // Bleu
    rectangleRGBA(renderer, 20, 75, 150, 105, 255, 255, 255, 200);
    stringRGBA(renderer, 53, 86, "TECH [T]", 255, 255, 255, 255); // Ecriture dans le Bouton
    // Calcul de la position idéale du texte (centré dans bouton) demandée à l'IA

    // Dessin du Bouton d'Aide [H]
    boxRGBA(renderer, 170, 75, 300, 105, 210, 105, 30, 255);
    rectangleRGBA(renderer, 170, 75, 300, 105, 255, 255, 255, 200);
    stringRGBA(renderer, 203, 86, "AIDE [H]", 255, 255, 255, 255);

    // Dessin du Bouton Fin de Tour (Rouge)
    boxRGBA(renderer, 20, 115, 300, 145, 150, 40, 40, 255);
    rectangleRGBA(renderer, 20, 115, 300, 145, 255, 255, 255, 200);
    stringRGBA(renderer, 47, 126, "======= FIN TOUR [F] =======", 255, 255, 255, 255);
}


void draw_panneau_biome_flottant(SDL_Renderer* renderer, Game* game, Position selection, int screenW) {
    if (selection.x == -1 || selection.y == -1) return;

    Tile* tuile = game->map->map[selection.y][selection.x];
    SDL_Color biome_color = get_biome_color(tuile->biome);

    int fog_lvl = tuile->fog_level;

    // Positionnement fixe en haut à droite de la fenêtre
    int w = 260;
    int h = 120;
    int x1 = screenW - w - 10; // À 10 pixels du bord droit (Fenetre de 1280px)
    int y1 = 10; // À 10 pixels du haut de l'écran

    boxRGBA(renderer, x1, y1, x1 + w, y1 + h, 10, 15, 25, 230);
    rectangleRGBA(renderer, x1, y1, x1 + w, y1 + h, biome_color.r, biome_color.g, biome_color.b, 255);

    if(fog_lvl == 0){
        // Bordure fine grise neutre pour ne pas trahir le biome caché
        rectangleRGBA(renderer, x1, y1, x1 + w, y1 + h, 130, 130, 130, 255);

        char l1[50], l2[50], l3[50], l4[50];
        sprintf(l1, "TERRAIN : ?? (%d, %d)", tuile->pos.x, tuile->pos.y);
        strcpy(l2,  "Nourriture / Production : ??");
        strcpy(l3,  "Science    / Or         : ??");
        strcpy(l4,  "PM requis               : ??");

        stringRGBA(renderer, x1 + 15, y1 + 15, l1, 255, 255, 255, 255); // Blanc
        stringRGBA(renderer, x1 + 15, y1 + 40, l2, 150, 150, 150, 255); // Gris
        stringRGBA(renderer, x1 + 15, y1 + 60, l3, 150, 150, 150, 255); // Gris
        stringRGBA(renderer, x1 + 15, y1 + 85, l4, 150, 150, 150, 255); // Gris
    }

    else{

        char l1[50], l2[50], l3[50], l4[50];
        sprintf(l1, "TERRAIN : %s (%d, %d)", get_biome_name(tuile->biome), tuile->pos.x, tuile->pos.y);

        // Statistiques des biomes
        switch (tuile->biome) {
            case 'P': // Plaine
                strcpy(l2, "Nourriture : +2"); strcpy(l3, "Production : +1"); strcpy(l4, "PM requis  : 1"); break;
            case 'F': // Forêt
                sprintf(l2, "Nourriture : %+d", 1 + game->tech_tree->bonus_food_forest); 
                strcpy(l3, "Production : +2"); strcpy(l4, "PM requis  : 2"); break;
            case 'M': // Montagne
                strcpy(l2, "Production : +3"); strcpy(l3, "Science    : +1"); strcpy(l4, "PM requis  : 3"); break;
            case 'E': // Eau
                strcpy(l2, "Nourriture : +1"); strcpy(l3, "Or         : +1"); strcpy(l4, "PM requis  : Bloque"); break;
            case 'D': // Désert
                strcpy(l2, "Nourriture : 0");  strcpy(l3, "Or         : +1"); strcpy(l4, "PM requis  : 1"); break;
            case 'T': // Toundra
                strcpy(l2, "Nourriture : +1"); strcpy(l3, "Production : +1"); strcpy(l4, "PM requis  : 1"); break;
            default:
                strcpy(l2, "Nourriture : 0");  strcpy(l3, "Production : 0");  strcpy(l4, "PM requis  : 1"); break;
        }

        stringRGBA(renderer, x1 + 15, y1 + 15, l1, 255, 255, 255, 255);
        stringRGBA(renderer, x1 + 15, y1 + 40, l2, 120, 255, 120, 255); // Vert clair
        stringRGBA(renderer, x1 + 15, y1 + 60, l3, 120, 220, 255, 255); // Bleu clair
        stringRGBA(renderer, x1 + 15, y1 + 85, l4, 255, 200, 100, 255); // Orange
    }
    
}



void draw_panneau_tuile_illuminee(SDL_Renderer* renderer, Game* game, Position selection, int screenW, int screenH, Unit * selected_unit) {
    if (selection.x == -1 || selection.y == -1) return;

    Tile* tuile = game->map->map[selection.y][selection.x];

    // On vérifie s'il n'y a ni ville, ni unité joueur, ni barbare, ni camp barbare
    if (!tuile->city_on && tuile->unit == NULL && tuile->barb_on == NULL && !tuile->camp_on) {
        return; 
    }

    // Taille du panneau
    int w = 950;
    int h = 120;
    int x1 = (screenW - w) / 2; 
    int y1 = screenH - h - 20;
    int x2 = x1 + w;
    int y2 = y1 + h;

    int fog_lvl = tuile->fog_level;

    //si tuile explorée alors on peut afficher les infos de la ville / camps barbare / unités
    if(fog_lvl >= 2){

        boxRGBA(renderer, x1, y1, x2, y2, 0, 0, 0, 230);
        rectangleRGBA(renderer, x1, y1, x2, y2, 200, 200, 200, 255);

        char txt1[100], txt2[100], txt3[100], txt4[100];
    
        // Moitié gauche = propriétés de la ville ou du camp barbare
        
        // Cas de la Ville (Cas 1 & Cas 3)
        if (tuile->city_on) {
            City* city = find_city_at_position(game, selection);
            if (city != NULL) {
                sprintf(txt1, "CITÉ : Population %d  |  Sante : %d PV", get_population(city), get_city_pv(city));
                sprintf(txt2, "Stocks : Bouffe %d | Prod %d", get_food(city), get_production(city));
                
                int nb_grenier = 0, nb_atelier = 0, nb_biblio = 0, nb_marche = 0, nb_caserne = 0, nb_muraille = 0, nb_phare = 0;
                BuildList* curr_b = city->buildings;

                while (curr_b != NULL) {
                    if (curr_b->data != NULL) {
                        switch (curr_b->data->type) {
                            case 'G': nb_grenier++; break;
                            case 'A': nb_atelier++; break;
                            case 'B': nb_biblio++; break;
                            case 'M': nb_marche++; break;
                            case 'C': nb_caserne++; break;
                            case 'R': nb_muraille++; break;
                            case 'P': nb_phare++; break;
                        }
                    }
                    curr_b = curr_b->next;
                }
                sprintf(txt3, "Quartiers : Gre:%d | Ate:%d | Bib:%d | Mar:%d | Cas:%d | Mur:%d | Pha:%d", nb_grenier, nb_atelier, nb_biblio, nb_marche,nb_caserne,nb_muraille,nb_phare);

                if (get_project(city) != NULL) {
                    sprintf(txt4, "Projet : %s (Reste : %d pr)", get_project_name(city), get_production_left(city));
                } else {
                    strcpy(txt4, "Projet : Aucun (Production perdue !)");
                }

                stringRGBA(renderer, x1 + 20, y1 + 15, txt1, 0, 255, 255, 255);  // Cyan
                stringRGBA(renderer, x1 + 20, y1 + 40, txt2, 255, 255, 255, 255);
                stringRGBA(renderer, x1 + 20, y1 + 65, txt3, 200, 255, 200, 255); // Vert
                stringRGBA(renderer, x1 + 20, y1 + 90, txt4, 255, 255, 100, 255); // Jaune
            }
        }
        // Cas du Camp Barbare (Cas 4)
        else if (tuile->camp_on) {
            stringRGBA(renderer, x1 + 20, y1 + 15, "CAMPEMENT ENNEMI : Camp Barbare", 255, 100, 100, 255); // Rouge
            stringRGBA(renderer, x1 + 20, y1 + 45, "Statut : Actif (Genere des troupes)", 255, 255, 255, 255);
            stringRGBA(renderer, x1 + 20, y1 + 75, "Ordre : Envoyez un Guerrier pour le raser", 255, 200, 100, 255);
        }
        // Cas de la Tuile Nue (Cas 2)
        else {
            stringRGBA(renderer, x1 + 20, y1 + 15, "STRUCTURE : Aucune infrastructure", 150, 150, 150, 255);
            stringRGBA(renderer, x1 + 20, y1 + 45, "Case inoccupée, disponible pour fonder", 200, 200, 200, 255);
            if (selected_unit) {
                stringRGBA(renderer, x1 + 20, y1 + 85, "-> Touche [M] pour ordonner le mouvement", 255, 165, 0, 255);
            } else {
                stringRGBA(renderer, x1 + 20, y1 + 85, "-> Cliquez sur une de vos troupes pour agir", 180, 180, 180, 255);
            }
        }

        // Moitié droite = Unités
        int offset_x = w/2+150; // On décale la colonne de droite à X1 la moitié de la largeur du panneau

        // Dessin d'une ligne verticale de séparation interne
        vlineRGBA(renderer, x1 + offset_x - 15, y1 + 10, y2 - 10, 80, 80, 80, 255);

        // Sous-Cas 1 : Unité alliée sur la case (Cas 3 ou Cas 5)
        if (tuile->unit != NULL) {
            Unit* u = tuile->unit;
            char nom_unite[30] = "";
        
            if (u->type == 'c'){
                strcpy(nom_unite, "Colon");
            }    

            else if (u->type == 'g') {
                strcpy(nom_unite, "Guerrier");
            }

            else if (u->type == 'e'){
                strcpy(nom_unite, "Eclaireur");
            }

            else{
                strcpy(nom_unite, "Inconnue");
            }

            sprintf(txt1, "GARNISON ALLIÉE : %s", nom_unite);
            sprintf(txt2, "Sante : %d / %d PV", u->pv, u->max_pv);
            sprintf(txt3, "Mouvement : %d / %d PM", u->pm, u->max_pm);
            sprintf(txt4, "Combat : ATK %d | DEF %d", u->atk, u->def);

            stringRGBA(renderer, x1 + offset_x, y1 + 15, txt1, 100, 255, 100, 255); // Vert
            stringRGBA(renderer, x1 + offset_x, y1 + 40, txt2, 255, 255, 255, 255);
            stringRGBA(renderer, x1 + offset_x, y1 + 65, txt3, 255, 255, 255, 255);
            stringRGBA(renderer, x1 + offset_x, y1 + 90, txt4, 255, 215, 0, 255); // Or
        }
        // Sous-Cas 2 : Barbare hostile sur la case (Cas 7)
        else if (tuile->barb_on != NULL) {
            Barbarian* b = tuile->barb_on;
            sprintf(txt1, "MENACE DETECTEE : Guerrier Barbare");
            sprintf(txt2, "Sante : %d / 15 PV", b->pv);
            sprintf(txt3, "Mouvement : %d / 3 PM", b->pm);
            sprintf(txt4, "Combat : ATK %d | DEF %d", b->atk, b->def);

            stringRGBA(renderer, x1 + offset_x, y1 + 15, txt1, 255, 50, 50, 255); // Rouge vif
            stringRGBA(renderer, x1 + offset_x, y1 + 40, txt2, 255, 255, 255, 255);
            stringRGBA(renderer, x1 + offset_x, y1 + 65, txt3, 255, 255, 255, 255);
            stringRGBA(renderer, x1 + offset_x, y1 + 90, txt4, 255, 100, 100, 255);
        }
        // Sous-Cas 3 : Case libre d'occupants
        else {
            stringRGBA(renderer, x1 + offset_x, y1 + 15, "OCCUPANT : Aucun", 150, 150, 150, 255);
            stringRGBA(renderer, x1 + offset_x, y1 + 45, "Pas d'unite militaire", 180, 180, 180, 255);
            stringRGBA(renderer, x1 + offset_x, y1 + 75, "ou civile en poste.", 180, 180, 180, 255);
        }
    }
}


//Panneau qui permet de guider sur les actions possibles
void draw_panneau_guide_actions(SDL_Renderer* renderer, Game* game) {
    int x1 = 10;
    int y1 = 160; // Positionné sous le tableau global
    int w = 560;
    int h = 535; 

    // Boîte de fond bleu et bordure fine grise
    boxRGBA(renderer, x1, y1, x1 + w, y1 + h, 15, 20, 30, 230);
    rectangleRGBA(renderer, x1, y1, x1 + w, y1 + h, 150, 150, 150, 255);

    // Titre principal
    stringRGBA(renderer, x1 + 15, y1 + 15, "=== ENCYCLOPEDIE DES ACTIONS ===", 0, 255, 255, 255);

    // CATEGORIE 1 : EXPANSION
    stringRGBA(renderer, x1 + 15, y1 + 45,  "[1] UNITES & EXPANSION (Sur Centre-Ville) :", 100, 255, 100, 255);
    stringRGBA(renderer, x1 + 25, y1 + 65,  "- Touche [C] : Planifier un Colon     (50 pr)", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 80,  "- Touche [G] : Planifier un Guerrier  (40 pr) [Req: Caserne]", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 95,  "- Touche [E] : Planifier un Eclaireur (40 pr) [Req: Expedition]", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 110, "- Touche [V] : Action Fonder Ville (Sur Colon selectionne)", 255, 255, 255, 255);

    // CATEGORIE 2 : BATIMENTS
    stringRGBA(renderer, x1 + 15, y1 + 140, "[2] BATIMENTS URBAINS (Touches [1] a [6]) :", 255, 215, 0, 255);
    
    // En-têtes du mini-tableau
    stringRGBA(renderer, x1 + 25,  y1 + 160, "  ID   Nom &  Cout     Effets / Bonus         Entretien & Req", 135, 206, 250, 255);
    
    stringRGBA(renderer, x1 + 25,  y1 + 180, "- [1] Grenier (30) : +3 Bouffe, Seuil/1.5    | 1 OR/t", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 195, "- [2] Atelier (40) : +3 Production           | 1 OR/t", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 210, "- [3] Biblio  (50) : +4 Science              | 1 OR/t [Ecriture]", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 225, "- [4] Marche  (40) : +3 Or                   | 1 OR/t [Commerce]", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 240, "- [5] Caserne (60) : Autorise les Guerriers  | 2 OR/t", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 255, "- [6] Muraille(80) : Sante Ville x2          | 2 OR/t [Maconnerie]", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 270, "- [7] Phare   (50) : +1 Champ de vision      | 1 OR/t [Mirador]", 255, 255, 255, 255);

    // CATEGORIE 3 : MILITAIRE
    stringRGBA(renderer, x1 + 15, y1 + 310, "[3] MANEUVRE & SÉCURITÉ MILITAIRE :", 255, 165, 0, 255);
    stringRGBA(renderer, x1 + 25, y1 + 330, "- Clic sur l'unite + [M]  : Activer la selection", 200, 200, 200, 255);
    stringRGBA(renderer, x1 + 25, y1 + 345, "- Clic sur la cible + [M] : Valider le deplacement", 200, 200, 200, 255);
    stringRGBA(renderer, x1 + 25, y1 + 360, "- Combat : Automatique en tentant d'entrer sur une case ennemie", 255, 150, 150, 255);
    
    // phrases sur barbares sur 2 lignes pour pas dépasser du cadre
    stringRGBA(renderer, x1 + 25, y1 + 375, "- Menace : Rasez les camps barbares pour gagner un bonus", 255, 150, 150, 255);
    stringRGBA(renderer, x1 + 36, y1 + 390, "  d'Or egal a 5x le nombre de camps detruits jusque-la.", 255, 150, 150, 255);

    // CATEGORIE 4 : RACCOURCIS PANNEAUX
    stringRGBA(renderer, x1 + 15, y1 + 430, "[4] ENCHAINEMENT DES TOURS & HUD :", 0, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 450, "- Touche [F] ou Bouton Rouge : Terminer le tour en cours", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 465, "- Touche [T] ou Bouton Bleu  : Consulter l'arbre technologique", 255, 255, 255, 255);

    // FERMETURE DU PANNEAU
    stringRGBA(renderer, x1 + 15, y1 + 505, "Pressez [H] ou cliquez sur AIDE pour fermer ce guide.", 130, 180, 255, 255);
}


// Dessine le fond du panneau de technologies
static void draw_tech_glass_panel(SDL_Renderer* renderer, int x1, int y1, int x2, int y2,
                                  Uint8 br, Uint8 bg, Uint8 bb) {
    roundedBoxRGBA(renderer, x1 + 5, y1 + 5, x2 + 5, y2 + 5, 14, 0, 0, 0, 120);
    roundedBoxRGBA(renderer, x1, y1, x2, y2, 14, 12, 18, 30, 235);
    roundedRectangleRGBA(renderer, x1, y1, x2, y2, 14, br, bg, bb, 230);
    roundedRectangleRGBA(renderer, x1 + 2, y1 + 2, x2 - 2, y2 - 2, 12, 255, 255, 255, 35);
}


// Dessine un séparateur discret pour structurer l'arbre technologique
static void draw_tech_separator(SDL_Renderer* renderer, int x1, int y, int x2) {
    hlineRGBA(renderer, x1, x2, y, 255, 255, 255, 35);
    hlineRGBA(renderer, x1, x2, y + 1, 0, 0, 0, 90);
}


// Dessine la barre de progression de la recherche active
static void draw_tech_progress_bar(SDL_Renderer* renderer, int x1, int y1, int w, int h,
                                   int current, int max) {
    int progress_w = 0;

    if (max > 0) {
        progress_w = (current * w) / max;
    }

    if (progress_w < 0) progress_w = 0;
    if (progress_w > w) progress_w = w;

    roundedBoxRGBA(renderer, x1, y1, x1 + w, y1 + h, 4, 20, 20, 25, 230);
    roundedRectangleRGBA(renderer, x1, y1, x1 + w, y1 + h, 4, 160, 160, 160, 130);

    if (progress_w > 0) {
        roundedBoxRGBA(renderer, x1 + 1, y1 + 1, x1 + progress_w, y1 + h - 1, 3, 255, 220, 70, 245);
    }
}


// Panneau de l'arbre technologique
void draw_panneau_arbre_tech(SDL_Renderer* renderer, Game* game, int screenW, int screenH) {
    if (renderer == NULL || game == NULL || game->tech_tree == NULL) return;

    TechTree* tree = game->tech_tree;

    int x1 = 45;
    int y1 = 45;
    int x2 = screenW - 45;
    int y2 = screenH - 45;

    int w = 165;
    int h = 58;

    // Fond principal de l'arbre technologique
    draw_tech_glass_panel(renderer, x1, y1, x2, y2, 0, 210, 255);

    stringRGBA(renderer, x1 + 28, y1 + 22, "ARBRE TECHNOLOGIQUE", 0, 235, 255, 255);

    char science_txt[80];
    snprintf(science_txt, sizeof(science_txt), "Science disponible : %d", game->science);
    stringRGBA(renderer, x1 + 28, y1 + 45, science_txt, 255, 230, 120, 255);

    draw_tech_separator(renderer, x1 + 25, y1 + 67, x2 - 25);

    // Positions fixes des technologies, indexees selon leur id
    int tech_x[11] = {
        x1 + 70,   // Depart
        x1 + 280,  // Chasse
        x1 + 280,  // Agriculture
        x1 + 280,  // Artisanat
        x1 + 280,  // Ecriture
        x1 + 280,  // Equitation
        x1 + 520,  // Irrigation
        x1 + 520,  // Maconnerie
        x1 + 520,  // Commerce
        x1 + 280,  // Mirador
        x1 + 520   // Expedition
    };

    int tech_y[11] = {
        y1 + 310,  // Depart
        y1 + 100,  // Chasse
        y1 + 180,  // Agriculture
        y1 + 260,  // Artisanat
        y1 + 340,  // Ecriture
        y1 + 420,  // Equitation
        y1 + 180,  // Irrigation
        y1 + 260,  // Maconnerie
        y1 + 340,  // Commerce
        y1 + 500,  // Mirador
        y1 + 500   // Expedition
    };

    // Sécurité si l'arbre contient plus de technologies que de positions dessinées
    int nb_positions = sizeof(tech_x) / sizeof(tech_x[0]);
    int nb_to_draw = tree->num_technologies;

    if (nb_to_draw > nb_positions) {
        nb_to_draw = nb_positions;
    }

    // Liens de dépendance entre technologies
    for (int i = 0; i < nb_to_draw; i++) {
        Technology* tech = &tree->technologies[i];

        for (int j = 0; j < tech->num_prerequisites; j++) {
            int prereq = tech->prerequisites[j];

            if (prereq < 0 || prereq >= nb_to_draw) {
                continue;
            }

            int start_x = tech_x[prereq] + w;
            int start_y = tech_y[prereq] + h / 2;
            int end_x = tech_x[i];
            int end_y = tech_y[i] + h / 2;

            // Décalage vertical des lignes pour éviter que plusieurs dépendances se superposent
            int offset = 0;

            if (i == 8 && prereq == 3) {
                offset = -3; // Artisanat -> Commerce
            } else if (i == 8 && prereq == 4) {
                offset = 10;  // Ecriture -> Commerce
            } else if (i == 7) {
                offset = -10;  // Artisanat -> Maconnerie
            } else if (i == 10) {
                offset = 6;   // Mirador -> Expedition
            }

            int mid_x = start_x + 35;

            // Trait lumineux en arrière-plan pour mieux distinguer les branches
            lineRGBA(renderer, start_x, start_y + offset - 1, mid_x, start_y + offset - 1, 0, 220, 255, 90);
            lineRGBA(renderer, mid_x, start_y + offset - 1, mid_x, end_y + offset - 1, 0, 220, 255, 90);
            lineRGBA(renderer, mid_x, end_y + offset - 1, end_x, end_y + offset - 1, 0, 220, 255, 90);

            // Trait principal
            lineRGBA(renderer, start_x, start_y + offset, mid_x, start_y + offset, 210, 210, 210, 210);
            lineRGBA(renderer, mid_x, start_y + offset, mid_x, end_y + offset, 210, 210, 210, 210);
            lineRGBA(renderer, mid_x, end_y + offset, end_x, end_y + offset, 210, 210, 210, 210);

            // Petit point d'arrivée pour clarifier la technologie cible
            filledCircleRGBA(renderer, end_x, end_y + offset, 3, 0, 220, 255, 190);
        }
    }

    // Affichage des cartes de technologies
    for (int i = 0; i < nb_to_draw; i++) {
        Technology* tech = &tree->technologies[i];

        Uint8 r = 80;
        Uint8 g = 80;
        Uint8 b = 85;

        if (tech->is_unlocked) {
            r = 40; g = 150; b = 75;        // Techno déjà recherchée
        } else if (game->active_research_id == tech->id) {
            r = 220; g = 165; b = 35;       // Recherche en cours
        } else if (can_research_tech(game, tree, tech->id)) {
            r = 40; g = 105; b = 180;       // Techno disponible
        }

        roundedBoxRGBA(renderer, tech_x[i] + 4, tech_y[i] + 5, tech_x[i] + w + 4, tech_y[i] + h + 5, 10, 0, 0, 0, 115);
        roundedBoxRGBA(renderer, tech_x[i], tech_y[i], tech_x[i] + w, tech_y[i] + h, 10, r, g, b, 225);
        roundedBoxRGBA(renderer, tech_x[i] + 2, tech_y[i] + 2, tech_x[i] + w - 2, tech_y[i] + 14, 8, 255, 255, 255, 35);
        roundedRectangleRGBA(renderer, tech_x[i], tech_y[i], tech_x[i] + w, tech_y[i] + h, 10, 235, 235, 235, 200);

        char title[80];

        if (tech->id == 0) {
            // La technologie de depart est la racine visuelle de l'arbre
            snprintf(title, sizeof(title), "%s", tech->name);

            int title_x = tech_x[i] + (w - (int)strlen(title) * 8) / 2;
            int title_y = tech_y[i] + 24;

            stringRGBA(renderer, title_x, title_y, title, 255, 255, 255, 255);
            stringRGBA(renderer, title_x + 1, title_y, title, 255, 255, 255, 255);
        } else {
            int display_id = (tech->id == 10) ? 0 : tech->id;
            snprintf(title, sizeof(title), "%d - %s", display_id, tech->name);

            char cost[80];
            if (game->active_research_id == tech->id && !tech->is_unlocked) {
                snprintf(cost, sizeof(cost), "%d / %d science", game->science, tech->science_cost);
            } else {
                snprintf(cost, sizeof(cost), "%d science", tech->science_cost);
            }

            // Les technologies sans prérequis affiché sont centrées verticalement dans leur carte
            if ((tech->id >= 1 && tech->id <= 5) || tech->id == 9) {
                stringRGBA(renderer, tech_x[i] + 10, tech_y[i] + 14, title, 255, 255, 255, 255);
                stringRGBA(renderer, tech_x[i] + 10, tech_y[i] + 31, cost, 225, 225, 225, 255);
            } else {
                stringRGBA(renderer, tech_x[i] + 10, tech_y[i] + 8, title, 255, 255, 255, 255);
                stringRGBA(renderer, tech_x[i] + 10, tech_y[i] + 24, cost, 225, 225, 225, 255);
            }

            // Affiche les prérequis seulement pour les technologies avancées
            if (!tech->is_unlocked &&
            game->active_research_id != tech->id &&
            tech->num_prerequisites > 0 &&
            tech->id >= 6 &&
            tech->id != 9) {
                char prereq_txt[80] = "Req : ";

                for (int k = 0; k < tech->num_prerequisites; k++) {
                    char tmp[20];

                    if (k > 0) {
                        strcat(prereq_txt, "+");
                    }

                    if (tech->prerequisites[k] == 2) {
                        snprintf(tmp, sizeof(tmp), "Agri");
                    } else if (tech->prerequisites[k] == 3) {
                        snprintf(tmp, sizeof(tmp), "Art");
                    } else if (tech->prerequisites[k] == 4) {
                        snprintf(tmp, sizeof(tmp), "Ecr");
                    } else if (tech->prerequisites[k] == 9) {
                        snprintf(tmp, sizeof(tmp), "Mir");
                    } else {
                        snprintf(tmp, sizeof(tmp), "%d", tech->prerequisites[k]);
                    }

                    strcat(prereq_txt, tmp);
                }

                stringRGBA(renderer, tech_x[i] + 10, tech_y[i] + 40, prereq_txt, 210, 210, 210, 255);
            }
        }

        // Barre de progression affichée uniquement pour la technologie en cours
        if (game->active_research_id == tech->id && !tech->is_unlocked && tech->science_cost > 0) {
            draw_tech_progress_bar(renderer, tech_x[i] + 10, tech_y[i] + h - 10, w - 20, 5,
                                   game->science, tech->science_cost);
        }
    }

    // Légende des couleurs
    int legend_x = x1 + 70;
    int legend_y = y1 + 105;

    roundedBoxRGBA(renderer, legend_x - 15, legend_y - 15, legend_x + 160, legend_y + 150, 10, 5, 10, 20, 145);
    roundedRectangleRGBA(renderer, legend_x - 15, legend_y - 15, legend_x + 160, legend_y + 150, 10, 255, 255, 255, 50);

    stringRGBA(renderer, legend_x, legend_y, "LEGENDE", 255, 255, 255, 255);

    roundedBoxRGBA(renderer, legend_x, legend_y + 25, legend_x + 18, legend_y + 43, 5, 40, 150, 70, 230);
    stringRGBA(renderer, legend_x + 30, legend_y + 30, "Debloquee", 220, 220, 220, 255);

    roundedBoxRGBA(renderer, legend_x, legend_y + 55, legend_x + 18, legend_y + 73, 5, 220, 165, 35, 230);
    stringRGBA(renderer, legend_x + 30, legend_y + 60, "En cours", 220, 220, 220, 255);

    roundedBoxRGBA(renderer, legend_x, legend_y + 85, legend_x + 18, legend_y + 103, 5, 40, 105, 180, 230);
    stringRGBA(renderer, legend_x + 30, legend_y + 90, "Disponible", 220, 220, 220, 255);

    roundedBoxRGBA(renderer, legend_x, legend_y + 115, legend_x + 18, legend_y + 133, 5, 80, 80, 85, 230);
    stringRGBA(renderer, legend_x + 30, legend_y + 120, "Bloquee", 220, 220, 220, 255);

    // Résumé rapide des effets pour aider le joueur à choisir sa recherche
    int info_x = x2 - 420;
    int info_y = y1 + 105;

    roundedBoxRGBA(renderer, info_x - 18, info_y - 15, x2 - 22, info_y + 430, 10, 5, 10, 20, 145);
    roundedRectangleRGBA(renderer, info_x - 18, info_y - 15, x2 - 22, info_y + 430, 10, 255, 255, 255, 50);

    stringRGBA(renderer, info_x, info_y, "EFFETS DES TECHNOLOGIES", 255, 255, 255, 255);
    draw_tech_separator(renderer, info_x, info_y + 18, x2 - 45);

    stringRGBA(renderer, info_x, info_y + 35,  "1 Chasse      : forets plus nourrissantes", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 50,  "                 +1 nourriture sur foret", 170, 170, 170, 255);

    stringRGBA(renderer, info_x, info_y + 75,  "2 Agriculture : villes plus efficaces", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 90,  "                 +10% nourriture globale", 170, 170, 170, 255);

    stringRGBA(renderer, info_x, info_y + 115, "3 Artisanat   : debloque les Guerriers", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 130, "                 +10% production globale", 170, 170, 170, 255);

    stringRGBA(renderer, info_x, info_y + 155, "4 Ecriture    : debloque la Bibliotheque", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 170, "                 +10% science globale", 170, 170, 170, 255);

    stringRGBA(renderer, info_x, info_y + 195, "5 Equitation  : unites plus mobiles", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 210, "                 +1 point de mouvement", 170, 170, 170, 255);

    stringRGBA(renderer, info_x, info_y + 235, "6 Irrigation  : gros bonus alimentaire", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 250, "                 +20% nourriture globale", 170, 170, 170, 255);

    stringRGBA(renderer, info_x, info_y + 275, "7 Maconnerie  : debloque les Murailles", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 290, "                 requis : Artisanat", 170, 170, 170, 255);

    stringRGBA(renderer, info_x, info_y + 315, "8 Commerce    : debloque le Marche", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 330, "                 requis : Artisanat + Ecriture", 170, 170, 170, 255);

    stringRGBA(renderer, info_x, info_y + 355, "9 Mirador     : debloque le Mirador", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 370, "                 ameliore la surveillance", 170, 170, 170, 255);

    stringRGBA(renderer, info_x, info_y + 395, "0 Expedition  : debloque l'Explorateur", 220, 220, 220, 255);
    stringRGBA(renderer, info_x, info_y + 410, "                 requis : Mirador", 170, 170, 170, 255);

    // Indication de fermeture
    stringRGBA(renderer, x1 + 28, y2 - 45, "Touches [1-9] et [0] pour choisir une technologie. [T] ou [ECHAP] pour fermer.", 180, 180, 180, 255);
}


// dessin du panneau de fin de partie
void draw_panneau_fin_partie(SDL_Renderer* renderer, Game* game, int game_status, int screenW, int screenH) {
    // Fond noir semi-transparent couvrant tout l'écran 
    boxRGBA(renderer, 0, 0, screenW, screenH, 10, 10, 15, 235);

    // Boîte centrale pour le message
    int w = 600;
    int h = 300;
    int x1 = (screenW - w) / 2;
    int y1 = (screenH - h) / 2;
    int x2 = x1 + w;
    int y2 = y1 + h;

    // Détermination de la couleur de la bordure et du titre selon le résultat
    Uint8 r = 255, g = 255, b = 255;
    char titre[100] = "";
    char sous_titre[100] = "";

    // Victoire Territoriale
    if (game_status == 1) { 
        r = 50; g = 205; b = 50; // Vert
        strcpy(titre, "VICTOIRE TERRITORIALE !");
        strcpy(sous_titre, "Votre empire s'est etendu sur plus de 10 cites !");
    } 

    // Victoire Technologique
    else if (game_status == 2) { 
        r = 30; g = 144; b = 255; // Bleu
        strcpy(titre, "VICTOIRE TECHNOLOGIQUE !");
        strcpy(sous_titre, "Vos savants ont complete l'arbre des sciences !");
    } 

    // Défaite
    else if (game_status == 3) { 
        r = 220; g = 20; b = 60; // Rouge
        strcpy(titre, "DEFAITE DE VOTRE CIVILISATION !");
        strcpy(sous_titre, "Le temps imparti est ecoule ou votre empire est mort.");
    }

    // Dessin du cadre central
    boxRGBA(renderer, x1, y1, x2, y2, 20, 25, 35, 255);
    rectangleRGBA(renderer, x1, y1, x2, y2, r, g, b, 255);

    // Affichage centré des textes 
    int len_titre = strlen(titre) * 8; //8 pixels par caractères
    stringRGBA(renderer, (screenW - len_titre) / 2, y1 + 40, titre, r, g, b, 255);
    
    int len_sous = strlen(sous_titre) * 8;
    stringRGBA(renderer, (screenW - len_sous) / 2, y1 + 80, sous_titre, 220, 220, 220, 255);

    // Affichage du score final
    char txt_score[50];
    sprintf(txt_score, "SCORE FINAL : %d POINTS", game_score(game));
    int len_score = strlen(txt_score) * 8;
    stringRGBA(renderer, (screenW - len_score) / 2, y1 + 130, txt_score, 255, 215, 0, 255); // Écrit en Doré

    // Dessin du bouton "QUITTER LE JEU" tout en bas du cadre
    // Position du bouton centré
    int bx1 = (screenW - 260) / 2;
    int by1 = y2 - 70;
    int bx2 = bx1 + 260;
    int by2 = by1 + 40;

    // Bouton rouge
    boxRGBA(renderer, bx1, by1, bx2, by2, 180, 50, 50, 255); 
    rectangleRGBA(renderer, bx1, by1, bx2, by2, 255, 255, 255, 200);
    
    // Texte centré à l'intérieur du bouton
    stringRGBA(renderer, bx1 + 86, by1 + 16, "QUITTER JEU", 255, 255, 255, 255);
}
