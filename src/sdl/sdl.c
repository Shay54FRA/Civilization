#include "../map/map.h"
#include "../tile/tile.h"
#include "../game/game.h"
#include "../configuration/configuration.h"
#include "../unit/unit.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>



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

void draw_map_sdl(SDL_Renderer* renderer, Game* game, int R, int h, Position position_actuelle, int cameraX, int cameraY) {

    for (int i = 0; i < game->map->height; i++) {
        for (int j = 0; j < game->map->length; j++) {
            Tile* tuile = game->map->map[i][j];


            //Calcule de la position puis on soustrait la caméra
            int x = j * (2 * h) + ((i % 2) * h) - cameraX + (1280 / 2);
            int y = i * (1.5f * R) - cameraY + (720 / 2);

            // On récupère la SDL_Color directement pour la mettre dans l'hexagone
            SDL_Color color = get_biome_color(tuile->biome);
            draw_hexagones(renderer, x, y, R, color);

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

            // Petit ajout pour les unités avec la version RGBA aussi
            if (tuile->unit != NULL) {
                filledCircleRGBA(renderer, x, y, R/2, 255, 0, 0, 255); // Rouge
            }
        }
    }
}

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
    int w = 600;
    int h = 110;
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
        stringRGBA(renderer, x1 + 20, y1 + 80, "-> MODE DEPLACEMENT ACTIF", 255, 150, 0, 255);
    } else if (tuile->unit) {
        stringRGBA(renderer, x1 + 20, y1 + 80, "Appuyez sur [M] pour selectionner", 100, 200, 255, 255);
    }

    // Colonne Droite : Infos unite (si presente)

    if (tuile->unit != NULL) {
        Unit* u = tuile->unit;
        sprintf(ligne1, "UNITE : %c", u->type);
        sprintf(ligne2, "PV : %d / %d", u->pv, u->max_pv); // Corrigé avec max_pv !
        sprintf(ligne3, "ATK : %d | DEF : %d | PM : %d / %d", u->atk, u->def, u->pm, u->max_pm); // Corrigé avec max_pm !

        stringRGBA(renderer, x1 + 320, y1 + 20, ligne1, 100, 255, 100, 255);
        stringRGBA(renderer, x1 + 320, y1 + 45, ligne2, 255, 255, 255, 255);
        stringRGBA(renderer, x1 + 320, y1 + 70, ligne3, 255, 200, 200, 255);
    }
    
    else {
        stringRGBA(renderer, x1 + 320, y1 + 20, "UNITE : Aucune", 150, 150, 150, 255);
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

    stringRGBA(renderer, 80, screenH - 90, "Appuyez sur [T] ou [ECHAP] pour revenir au jeu", 180, 180, 180, 255);
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


void run_game_sdl(Game * game) {

    //INITIALISATION DE SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Erreur Init : %s", SDL_GetError());
        return;
    }

    //taille window
    int width = 1280;
    int height = 720;

    //CREATION DE LA WINDOW
    SDL_Window * window = SDL_CreateWindow("Civilisation PP2ix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Position position_actuelle = {-1, -1}; //aucun hexagone ne s'illumine initialement

    //pour la boucle event
    int running = 1;
    SDL_Event event;

    // Constantes pour les hexagones
    int R = 40; 
    int h = (int)(R * 0.866f);

    //Variables caméra qui commencent au cnetre (0,0)
    int cameraX = 0;
    int cameraY = 0;

    //On calcule la taille maximale de la carte en pixels pour bloquer la caméra (qu'elle n'aille pas dans des énormes coordonnées)
    int maxWidth = game->map->length * (2*h);
    int maxHeight = game->map->height * (1.5f * R);

    // variables d'état du jeu
    Unit* selected_unit = NULL;
    char last_message[100] = "Bienvenue dans Civ PP2ix !";
    int show_arbre_tech = 0; // arbre techno initialement pas affiché

    while(running) {

        //GESTION DES ÉVÉNEMENTS
        while(SDL_PollEvent(&event)) { 

            switch(event.type) {

                case SDL_QUIT: running = 0; 
                break;

                case SDL_KEYUP: {
                    Position next_position = position_actuelle;
                    
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        if (show_arbre_tech) show_arbre_tech = 0;
                        else running = 0;
                    }

                    //arbre technologique (touche T)
                    if (event.key.keysym.sym == SDLK_t) {
                        show_arbre_tech = !show_arbre_tech;
                        snprintf(last_message, sizeof(last_message), "Menu arbre technologique");
                    }

                    // Passer le tour au clavier (touche F)
                    if (event.key.keysym.sym == SDLK_f && !show_arbre_tech) {
                        game->active_turn++;
                        snprintf(last_message, sizeof(last_message), "Tour suivant actif");
                    }

                    // Unités et Ville
                    if (position_actuelle.x != -1 && !show_arbre_tech) {
                        Tile* tuile_suivante = game->map->map[position_actuelle.y][position_actuelle.x];

                        // Selectionner ou Déplacer (Touche M)
                        if (event.key.keysym.sym == SDLK_m) {

                            if (selected_unit == NULL) {

                                if (tuile_suivante && tuile_suivante->unit) {
                                    selected_unit = tuile_suivante->unit;
                                    snprintf(last_message, sizeof(last_message), "Unité selectionnée, cliquez sur la case cible + la touche M");
                                }

                                else {
                                    snprintf(last_message, sizeof(last_message), "Aucune unité sur cette case");
                                }

                            } 
                            
                            else {
                                // Si il y a une unité dessus 
                                MoveResult res = move_unit_step(game, selected_unit, position_actuelle);
                                get_move_message(res, last_message, sizeof(last_message));
                                selected_unit = NULL; // On relâche l'unité
                            }
                        }

                        // Commande [V] : Fonder une ville
                        if (event.key.keysym.sym == SDLK_v) {
                            if (tuile_suivante->unit && tuile_suivante->unit->type == 'c') {
                                //Creattion d'une ville (il faudrait appeler une fct creation ville)
                                tuile_suivante->city_on = true; 
                                snprintf(last_message, sizeof(last_message), "Ville fondee avec succes !");
                            } else {
                                snprintf(last_message, sizeof(last_message), "Seul un Colon peut fonder une ville");
                            }
                        }
                    }
                
                    // On vérifie les limites AVANT de valider le mouvement
                    if (next_position.x >= 0 && next_position.x < game->map->length &&
                        next_position.y >= 0 && next_position.y < game->map->height) {
                        position_actuelle = next_position;
                    }

                    break;
                }

                case SDL_MOUSEBUTTONUP: {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    // On chope les coordonnées exactes du clic
                    int mx = event.button.x;
                    int my = event.button.y;
                    
                    // gestion des clics sur les boutons des panneaux d'abord
                    if (mx >= 20 && mx <= 150 && my >= 75 && my <= 105) {
                        show_arbre_tech = !show_arbre_tech;
                        break;
                    }

                    if (mx >= 170 && mx <= 300 && my >= 75 && my <= 105 && !show_arbre_tech) {
                        game->active_turn++;
                        snprintf(last_message, sizeof(last_message), "Tour suivant.");
                        break;
                    }

                    // 2. CLIC SUR LE TERRAIN (Seulement si l'arbre techno est fermé)
                    if (!show_arbre_tech) {
                        Position nouvelle_selection = position_hexagone(mx, my, R, h, cameraX, cameraY, game);

                        if (nouvelle_selection.x == position_actuelle.x && nouvelle_selection.y == position_actuelle.y) {
                            position_actuelle.x = -1; position_actuelle.y = -1;
                            selected_unit = NULL; // Annule la sélection d'unité en cours
                        } 
                        
                        else {
                            position_actuelle = nouvelle_selection;
                        }
                    }
                }
                break;
                
            }
        }
    }

    // CAMERA 
    if(!show_arbre_tech){ //seulement si arbre tech pas affiché
        // On demande à SDL où est la souris en temps réel sur l'écran
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        // Si elle est à moins de 30 pixels d'un bord, on décale la caméra
        if (mouseX < 30)   cameraX -= 50; // Bord gauche
        if (mouseX > 1250) cameraX += 50; // Bord droit
        if (mouseY < 30)   cameraY -= 50; // Bord haut
        if (mouseY > 690)  cameraY += 50; // Bord bas 

        //Garde-fous de la caméra (en empêche la caméra de partir dans le négatif ou d'aller trop loin)
        if (cameraX < 0) cameraX = 0;
        if (cameraY < 0) cameraY = 0;
        if (cameraX > maxWidth) cameraX = maxWidth;
        if (cameraY > maxHeight) cameraY = maxHeight;
    }

    //RENDU
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer); 

    //Dessin de la map avec la caméra
    draw_map_sdl(renderer, game, R, h, position_actuelle, cameraX, cameraY);

    //Dessin du tableau d'affichage global
    draw_panneau_global(renderer, game);

    //dessin panneau des messages 
    draw_panneau_message_action(renderer,last_message,width);

    // Dessin du panneau de la tuile illuminee en bas, qui s'affiche dynamiquement
    draw_panneau_tuile_illuminee(renderer, game, position_actuelle, width, height,selected_unit);

    // Si l'arbre techno est ouvert, on l'affiche par-dessus tout le reste
    if (show_arbre_tech) {
        draw_panneau_arbre_tech(renderer, game, width, height);
    }

    SDL_RenderPresent(renderer);

    //Limiteur de vitesse : 1000ms / 16ms = 60 images par secondes
    //permet de forcer le processeur à attendre -> diminution de la chauffe du CPU
    //SDL_Delay(16);
}

    //NETTOYAGE
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
}
