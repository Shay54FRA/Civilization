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

    // Positionnement fixe en haut à droite de la fenêtre
    int w = 260;
    int h = 120;
    int x1 = screenW - w - 10; // À 10 pixels du bord droit (Fenetre de 1280px)
    int y1 = 10; // À 10 pixels du haut de l'écran

    boxRGBA(renderer, x1, y1, x1 + w, y1 + h, 10, 15, 25, 230);
    rectangleRGBA(renderer, x1, y1, x1 + w, y1 + h, biome_color.r, biome_color.g, biome_color.b, 255);

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



void draw_panneau_tuile_illuminee(SDL_Renderer* renderer, Game* game, Position selection, int screenW, int screenH, Unit * selected_unit) {
    if (selection.x == -1 || selection.y == -1) return;

    Tile* tuile = game->map->map[selection.y][selection.x];

    // On vérifie s'il n'y a ni ville, ni unité joueur, ni barbare, ni camp barbare
    if (!tuile->city_on && tuile->unit == NULL && tuile->barb_on == NULL && !tuile->camp_on) {
        return; 
    }

    // Taille du panneau
    int w = 800;
    int h = 120;
    int x1 = (screenW - w) / 2; 
    int y1 = screenH - h - 20;
    int x2 = x1 + w;
    int y2 = y1 + h;

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
            
            int nb_grenier = 0, nb_atelier = 0, nb_biblio = 0, nb_marche = 0, nb_caserne = 0, nb_muraille = 0;
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
                    }
                }
                curr_b = curr_b->next;
            }
            sprintf(txt3, "Quartiers : Gre:%d | Ate:%d | Bib:%d | Mar:%d", nb_grenier, nb_atelier, nb_biblio, nb_marche);

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
    int offset_x = w/2; // On décale la colonne de droite à X1 la moitié de la largeur du panneau

    // Dessin d'une ligne verticale de séparation interne
    vlineRGBA(renderer, x1 + offset_x - 15, y1 + 10, y2 - 10, 80, 80, 80, 255);

    // Sous-Cas 1 : Unité alliée sur la case (Cas 3 ou Cas 5)
    if (tuile->unit != NULL) {
        Unit* u = tuile->unit;
        sprintf(txt1, "GARNISON ALLIÉE : %s", (u->type == 'c') ? "Colon" : "Guerrier");
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
        sprintf(txt1, "MENACE DETECTÉE : Guerrier Barbare");
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


//Panneau qui permet de guider sur les actions possibles
void draw_panneau_guide_actions(SDL_Renderer* renderer, Game* game) {
    int x1 = 10;
    int y1 = 160; // Positionné sous le tableau global
    int w = 560;
    int h = 465; 

    // Boîte de fond bleu et bordure fine grise
    boxRGBA(renderer, x1, y1, x1 + w, y1 + h, 15, 20, 30, 230);
    rectangleRGBA(renderer, x1, y1, x1 + w, y1 + h, 150, 150, 150, 255);

    // Titre principal
    stringRGBA(renderer, x1 + 15, y1 + 15, "=== ENCYCLOPEDIE DES ACTIONS ===", 0, 255, 255, 255);

    // CATEGORIE 1 : EXPANSION
    stringRGBA(renderer, x1 + 15, y1 + 45, "[1] UNITES & EXPANSION (Sur Centre-Ville) :", 100, 255, 100, 255);
    stringRGBA(renderer, x1 + 25, y1 + 65, "- Touche [C] : Planifier un Colon    (50 pr)", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 80, "- Touche [G] : Planifier un Guerrier (40 pr) [Req: Caserne]", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 95, "- Touche [V] : Action Fonder Ville (Sur Colon selectionne)", 255, 255, 255, 255);

    // CATEGORIE 2 : BATIMENTS
    stringRGBA(renderer, x1 + 15, y1 + 125, "[2] BATIMENTS URBAINS (Touches [1] a [6]) :", 255, 215, 0, 255);
    
    // En-têtes du mini-tableau pour guider l'œil
    stringRGBA(renderer, x1 + 25,  y1 + 145, "  ID   Nom &  Cout     Effets / Bonus         Entretien & Req", 135, 206, 250, 255);
    
    stringRGBA(renderer, x1 + 25,  y1 + 165, "- [1] Grenier (30) : +3 Bouffe, Seuil/1.5    | 1 OR/t", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 180, "- [2] Atelier (40) : +3 Production           | 1 OR/t", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 195, "- [3] Biblio  (50) : +4 Science              | 1 OR/t [Ecriture]", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 210, "- [4] Marche  (40) : +3 Or                   | 1 OR/t [Commerce]", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 225, "- [5] Caserne (60) : Autorise les Guerriers  | 2 OR/t", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25,  y1 + 240, "- [6] Muraille(80) : Sante Ville x2          | 2 OR/t [Maconnerie]", 255, 255, 255, 255);

    // CATEGORIE 3 : MILITAIRE
    stringRGBA(renderer, x1 + 15, y1 + 270, "[3] MANEUVRE & SÉCURITÉ MILITAIRE :", 255, 165, 0, 255);
    stringRGBA(renderer, x1 + 25, y1 + 290, "- Clic sur l'unite + [M]  : Activer la selection", 200, 200, 200, 255);
    stringRGBA(renderer, x1 + 25, y1 + 305, "- Clic sur la cible + [M] : Valider le deplacement", 200, 200, 200, 255);
    stringRGBA(renderer, x1 + 25, y1 + 320, "- Combat : Automatique en tentant d'entrer sur une case ennemie", 255, 150, 150, 255);
    
    // Phrase des barbares sur 2 lignes pour éviter de dépasser du cadre
    stringRGBA(renderer, x1 + 25, y1 + 335, "- Menace : Rasez les camps barbares pour gagner un bonus", 255, 150, 150, 255);
    stringRGBA(renderer, x1 + 36, y1 + 350, "  d'Or egal a 5x le nombre de camps detruits jusque-la.", 255, 150, 150, 255);

    // CATEGORIE 4 : RACCOURCIS PANNEAUX
    stringRGBA(renderer, x1 + 15, y1 + 380, "[4] ENCHAINEMENT DES TOURS & HUD :", 0, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 400, "- Touche [F] ou Bouton Rouge : Terminer le tour en cours", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 415, "- Touche [T] ou Bouton Bleu  : Consulter l'arbre technologique", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 15, y1 + 445, "Pressez [H] ou cliquez sur AIDE pour fermer ce guide.", 130, 180, 255, 255);
}

// Panneau de l'arbre technologique
void draw_panneau_arbre_tech(SDL_Renderer* renderer, Game* game, int screenW, int screenH) {
    // Fond sombre de la fenêtre de recherche
    boxRGBA(renderer, 50, 50, screenW - 50, screenH - 50, 15, 20, 35, 245);
    rectangleRGBA(renderer, 50, 50, screenW - 50, screenH - 50, 0, 200, 255, 255);

    // Titre et contenu textuel fixe (en attendant le module dynamique de tes collègues)
    stringRGBA(renderer, 80, 80, "=== ARBRE DES TECHNOLOGIES (MODULE COLLÈGUES) ===", 0, 255, 255, 255);
    
    stringRGBA(renderer, 80, 130, "- Chasse      (50 Science) -> +1 Nourriture sur les Forets", 200, 200, 200, 255);
    stringRGBA(renderer, 80, 155, "- Agriculture (60 Science) -> +10%% Nourriture globale", 200, 200, 200, 255);
    stringRGBA(renderer, 80, 180, "- Artisanat   (70 Science) -> Debloque Guerrier & +10%% Prod", 200, 200, 200, 255);
    stringRGBA(renderer, 80, 205, "- Ecriture    (80 Science) -> Debloque Bibliotheque & +10%% Sci", 200, 200, 200, 255);
    stringRGBA(renderer, 80, 230, "- Maçonnerie  (100 Science) -> Debloque Muraille", 200, 200, 200, 255);
    stringRGBA(renderer, 80, 255, "- Commerce    (90 Science) -> Debloque Marche & +10%% Or", 200, 200, 200, 255);
    stringRGBA(renderer, 80, 280, "- Equitation  (100 Science) -> +1 PM pour toutes les unites", 200, 200, 200, 255);

    // Instruction de fermeture
    stringRGBA(renderer, 80, screenH - 90, "Appuyez sur [T] ou [ECHAP] pour masquer l'interface", 180, 180, 180, 255);
}