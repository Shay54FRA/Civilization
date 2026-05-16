#include "../map/map.h"
#include "../tile/tile.h"
#include "../game/game.h"
#include "../configuration/configuration.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>


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

void draw_panneau_global(SDL_Renderer* renderer, Game* game) {
    // Boîte noire semi-transparent (X=10, Y=10, Largeur=310, Hauteur=80)
    boxRGBA(renderer, 10, 10, 310, 80, 0, 0, 0, 200);
    
    // Bordure fine grise pour faire stylé
    rectangleRGBA(renderer, 10, 10, 310, 80, 150, 150, 150, 255);

    char txt_tour[50];
    char txt_ressources[100];

    sprintf(txt_tour, "TOUR : %d / %d", game->active_turn, game->configuration->t);
    sprintf(txt_ressources, "OR : %d  |  SCIENCE : %d", game->gold, game->science);

    // Affichage des textes
    stringRGBA(renderer, 25, 25, txt_tour, 255, 255, 255, 255);
    stringRGBA(renderer, 25, 50, txt_ressources, 255, 215, 0, 255); // Écrit en couleur dorée
}

void draw_panneau_tuile_illuminee(SDL_Renderer* renderer, Game* game, Position selection, int screenW, int screenH) {
    // Si aucune tuile n'est sélectionnée, on ne dessine rien
    if (selection.x == -1 || selection.y == -1) {
        return;
    }

    // Calcul pour centrer le panneau de 500px en bas de l'écran
    int w = 500;
    int h = 80;
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

    // On utilise get_biome_name pour afficher textuellement le biome (Plaine, Eau...)
    sprintf(ligne1, "COORDONNEES : (%d, %d)  |  BIOME : %s", 
            tuile->pos.x, tuile->pos.y, get_biome_name(tuile->biome));
            
    sprintf(ligne2, "VILLE : %s  |  UNITE : %s", 
            tuile->city_on ? "Oui" : "Aucune", 
            (tuile->unit != NULL) ? "Presente" : "Aucune");

    // Affichage des textes
    stringRGBA(renderer, x1 + 20, y1 + 20, ligne1, 255, 255, 255, 255);
    stringRGBA(renderer, x1 + 20, y1 + 45, ligne2, 200, 200, 200, 255);
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
    SDL_Window * window = SDL_CreateWindow("Civ PP2ix", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
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

    while(running) {

        //GESTION DES ÉVÉNEMENTS
        while(SDL_PollEvent(&event)) { 

            switch(event.type) {

                case SDL_QUIT: running = 0; break;

                case SDL_KEYUP: {
                    Position next_position = position_actuelle;
                    switch (event.key.keysym.sym) {
                        case SDLK_z: next_position.y--; break;
                        case SDLK_s: next_position.y++; break;
                        case SDLK_q: next_position.x--; break;
                        case SDLK_d: next_position.x++; break;
                        case SDLK_ESCAPE: running = 0; break;
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
                    
                    // On utilise notre fonction pour savoir sur quelle tuile on a cliqué
                    Position nouvelle_selection = position_hexagone(mx, my, R, h, cameraX, cameraY, game);

                    // Si on reclique sur la tuile déjà illuminée -> on désélectionne la tuile
                    if (nouvelle_selection.x == position_actuelle.x && nouvelle_selection.y == position_actuelle.y) {
                        position_actuelle.x = -1;
                        position_actuelle.y = -1; 
                    } 
                    // Sinon, on applique la nouvelle sélection
                    else {
                        position_actuelle = nouvelle_selection;
                    }
                }

                break;
                }
            }
        }

        /*
        Quand c'était déplacement avec zqsd :
        //MISE À JOUR DE LA CAMÉRA
        int camX = position_actuelle.x * (2 * h) + ((position_actuelle.y % 2) * h);
        int camY = position_actuelle.y * (1.5f * R);
        */

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


        //RENDU
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer); 

        //Dessin de la map avec la caméra
        draw_map_sdl(renderer, game, R, h, position_actuelle, cameraX, cameraY);

        //Dessin du tableau d'affichage global
        draw_panneau_global(renderer, game);

        // Dessin du panneau de la tuile illuminee en bas, qui s'affiche dynamiquement
        draw_panneau_tuile_illuminee(renderer, game, position_actuelle, width, height);


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
