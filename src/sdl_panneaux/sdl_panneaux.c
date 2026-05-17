#include "sdl_panneaux.h"
#include "../sdl_map/sdl_map.h" 
#include "../game/game.h"
#include "../tile/tile.h"
#include "../unit/unit.h"
#include "../configuration/configuration.h"
#include <SDL2_gfxPrimitives.h>
#include <stdio.h>


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
    // Boîte noire semi-transparent (X=10, Y=10, Largeur=310, Hauteur=80)
    boxRGBA(renderer, 10, 10, 310, 115, 0, 0, 0, 200);
    
    // Bordure fine grise pour faire stylé
    rectangleRGBA(renderer, 10, 10, 310, 115, 150, 150, 150, 255);

    char txt_tour[50];
    char txt_ressources[100];

    sprintf(txt_tour, "TOUR : %d / %d", game->active_turn, game->configuration->t);
    sprintf(txt_ressources, "OR : %d  |  SCIENCE : %d", game->gold, game->science);

    // Affichage des textes
    stringRGBA(renderer, 25, 25, txt_tour, 255, 255, 255, 255);
    stringRGBA(renderer, 25, 50, txt_ressources, 255, 215, 0, 255); // Écrit en couleur dorée

    // Dessin du Bouton TECHNOLOGIES (Bleu)
    boxRGBA(renderer, 20, 75, 150, 105, 30, 80, 150, 255);
    rectangleRGBA(renderer, 20, 75, 150, 105, 255, 255, 255, 200);
    stringRGBA(renderer, 40, 85, "TECH [T]", 255, 255, 255, 255);

    // Dessin du Bouton FIN DE TOUR (Rouge)
    boxRGBA(renderer, 170, 75, 300, 105, 150, 40, 40, 255);
    rectangleRGBA(renderer, 170, 75, 300, 105, 255, 255, 255, 200);
    stringRGBA(renderer, 195, 85, "FIN TOUR [F]", 255, 255, 255, 255);
}

void draw_panneau_tuile_illuminee(SDL_Renderer* renderer, Game* game, Position selection, int screenW, int screenH, Unit * selected_unit) {
    // Si aucune tuile n'est sélectionnée, on ne dessine rien
    if (selection.x == -1 || selection.y == -1) {
        return;
    }

    // Calcul pour centrer le panneau de 500px en bas de l'écran
    int w = 650;
    int h = 120;
    int x1 = (screenW - w) / 2;
    int y1 = screenH - h - 20; // À 20 pixels du bas de l'écran
    int x2 = x1 + w;
    int y2 = y1 + h;

    // Récupération des données de la tuile cliquée
    Tile* tuile = game->map->map[selection.y][selection.x];

    // On récupère la couleur du biome pour l'appliquer à la bordure
    SDL_Color biome_color = get_biome_color(tuile->biome);

    // Fond noir semi-transparent
    boxRGBA(renderer, x1, y1, x2, y2, 0, 0, 0, 220);

    // Bordure assortie à la couleur du biome de la tuile !
    rectangleRGBA(renderer, x1, y1, x2, y2, biome_color.r, biome_color.g, biome_color.b, 255);

    char ligne1[100];
    char ligne2[100];
    char ligne3[100];

    // Colonne Gauche : Infos terrain

    // On utilise get_biome_name pour afficher textuellement le biome (Plaine, Eau...)
    sprintf(ligne1, "TERRAIN : %s (%d, %d)", get_biome_name(tuile->biome), tuile->pos.x, tuile->pos.y);
    sprintf(ligne2, "VILLE   : %s", tuile->city_on ? "Oui" : "Non");
    stringRGBA(renderer, x1 + 20, y1 + 20, ligne1, 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 20, y1 + 45, ligne2, 200, 200, 200, 255);

    // Indication du mode de sélection
    if (selected_unit) {
        stringRGBA(renderer, x1 + 20, y1 + 80, "-> ORDRE : [M] pour deplacer ici", 255, 150, 0, 255);
    } 

    else if (tuile->city_on) {
        stringRGBA(renderer, x1 + 20, y1 + 80, "-> PROD  : [G] Guerrier | [C] Colon", 100, 255, 100, 255);
    }

    else if(tuile->unit){
        if (tuile->unit->type == 'c') {
            stringRGBA(renderer, x1 + 20, y1 + 80, "-> COLON : [M] Bouger | [V] Fonder", 100, 255, 100, 255);
        } 
        else {
            stringRGBA(renderer, x1 + 20, y1 + 80, "-> TROUPE: [M] Selectionner l'unite", 100, 200, 255, 255);
        }
    }

    else {
        stringRGBA(renderer, x1 + 20, y1 + 80, "-> Case libre", 150, 150, 150, 255);
    }

    // Colonne Droite : Infos Ville ou Unite (si presente) (Priorité à la Ville)

    // CAS 1 : Il y a une ville sur la case
    if (tuile->city_on) {
        City* city = find_city_at_position(game, selection);
        if (city != NULL) {
            int pop = get_population(city);
            int pv  = get_city_pv(city);
            int stock_food = get_food(city);
            int delta_food = get_new_food(city);
            int stock_prod = get_production(city);
            int delta_prod = get_new_prod(city);

            sprintf(ligne1, "VILLE : Population %d  |  PV : %d", pop, pv);
            sprintf(ligne2, "NOURRITURE : %d (%+d/tour)", stock_food, delta_food);
            sprintf(ligne3, "PRODUCTION : %d (%+d/tour)", stock_prod, delta_prod);

            stringRGBA(renderer, x1 + 340, y1 + 20, ligne1, 0, 255, 255, 255); // Cyan pour le titre de la ville
            stringRGBA(renderer, x1 + 340, y1 + 40, ligne2, 255, 255, 255, 255);
            stringRGBA(renderer, x1 + 340, y1 + 60, ligne3, 255, 255, 255, 255);

            // Affichage de l'avancement du projet
            if (get_project(city) != NULL) {
                char pr_ligne[100];
                sprintf(pr_ligne, "PROJET : %s (Reste : %d)", get_project_name(city), get_production_left(city));
                stringRGBA(renderer, x1 + 340, y1 + 82, pr_ligne, 255, 255, 100, 255); // Jaune pour le projet
            } 
            else {
                stringRGBA(renderer, x1 + 340, y1 + 82, "PROJET : Aucun (Prod perdue !)", 255, 100, 100, 255); // Rouge si vide
            }
        }
    }

    // CAS 2 : Pas de ville, mais il y a une unité
    else if (tuile->unit != NULL) {
        Unit* u = tuile->unit;
        sprintf(ligne1, "UNITE : %s [%c]", (u->type == 'b' || u->type == 'B') ? "Barbare" : (u->type == 'c' ? "Colon" : "Guerrier"), u->type);
        sprintf(ligne2, "PV : %d / %d", u->pv, u->max_pv); 
        sprintf(ligne3, "ATK : %d | DEF : %d | PM : %d / %d", u->atk, u->def, u->pm, u->max_pm);

        stringRGBA(renderer, x1 + 340, y1 + 20, ligne1, 100, 255, 100, 255); // Vert pour l'unité
        stringRGBA(renderer, x1 + 340, y1 + 45, ligne2, 255, 255, 255, 255);
        stringRGBA(renderer, x1 + 340, y1 + 70, ligne3, 255, 200, 200, 255);
    }

    // CAS 3 : Ni ville, ni unité
    else {
        stringRGBA(renderer, x1 + 340, y1 + 20, "CONTENU : Aucun", 150, 150, 150, 255);
    }
}


// Dessin de l'arbre technologique
void draw_panneau_arbre_tech(SDL_Renderer* renderer, Game* game, int screenW, int screenH) {
    boxRGBA(renderer, 50, 50, screenW - 50, screenH - 50, 15, 20, 35, 245);
    rectangleRGBA(renderer, 50, 50, screenW - 50, screenH - 50, 0, 200, 255, 255);

    stringRGBA(renderer, 80, 80, "=== ARBRE DES TECHNOLOGIES ===", 0, 255, 255, 255);
    stringRGBA(renderer, 80, 130, "- Agriculture [Debloque]", 100, 255, 100, 255);
    stringRGBA(renderer, 80, 160, "- Elevage     (Cout: 15 Science)", 255, 255, 255, 255);
    stringRGBA(renderer, 80, 190, "- Poterie     (Cout: 20 Science)", 255, 255, 255, 255);
    stringRGBA(renderer, 80, 220, "- Maçonnerie  [Muraille] (Cout: 30 Science)", 255, 255, 255, 255);

    stringRGBA(renderer, 80, screenH - 90, "Appuyez sur [T] ou [ECHAP] pour fermer l'arbre", 180, 180, 180, 255);
}


//Panneau qui permet de guider sur les actions possibles
void draw_panneau_guide_actions(SDL_Renderer* renderer, Game* game) {
    int x1 = 10;
    int y1 = 135; // Positionné sous le tableau global
    int w = 420;
    int h = 320; // Légèrement agrandi pour accueillir le texte

    // Boîte noire translucide et bordure grise
    boxRGBA(renderer, x1, y1, x1 + w, y1 + h, 0, 0, 0, 210);
    rectangleRGBA(renderer, x1, y1, x1 + w, y1 + h, 150, 150, 150, 255);

    // Titre principal
    stringRGBA(renderer, x1 + 15, y1 + 15, "=== ENCYCLOPEDIE DES ACTIONS ===", 0, 255, 255, 255);

    // CATEGORIE 1 : EXPANSION
    stringRGBA(renderer, x1 + 15, y1 + 45, "[1] FONDATION & EXPANSION :", 100, 255, 100, 255);
    stringRGBA(renderer, x1 + 25, y1 + 65, "- Clic Ville + Touche [C] -> Colon", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 80, "  Cout: 50 Prod | Entretien: 0 Or", 200, 200, 200, 255);
    stringRGBA(renderer, x1 + 25, y1 + 95, "- Selection Colon + [V]   -> Fonder Ville", 255, 255, 255, 255);

    // CATEGORIE 2 : MILITAIRE
    stringRGBA(renderer, x1 + 15, y1 + 125, "[2] ARMEE & DEFENSE :", 255, 100, 100, 255);
    stringRGBA(renderer, x1 + 25, y1 + 145, "- Clic Ville + Touche [G] -> Guerrier", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 160, "  Cout: 40 Prod | Entretien: 1 Or/t", 200, 200, 200, 255);
    stringRGBA(renderer, x1 + 25, y1 + 175, "  REQUIS : La ville doit avoir une Caserne", 255, 255, 100, 255);

    // CATEGORIE 3 : CONTROLES
    stringRGBA(renderer, x1 + 15, y1 + 205, "[3] MANEUVRE DES UNITES :", 255, 165, 0, 255);
    stringRGBA(renderer, x1 + 25, y1 + 225, "- Selectionner : Clic case + [M]", 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 240, "- Deplacer     : Clic cible + [M]", 255, 255, 255, 255);

    // CATEGORIE 4 : FIN DE CYCLE
    stringRGBA(renderer, x1 + 15, y1 + 275, "[4] APPLIQUER LES TOURS :", 0, 255, 255, 255);
    stringRGBA(renderer, x1 + 25, y1 + 295, "  Pressez [F] pour finir votre tour", 255, 100, 100, 255);
}

