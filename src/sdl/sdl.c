#include "../game/game.h"
#include "../configuration/configuration.h"
#include "../building/building.h"
#include "../unit/unit.h"
#include "../sdl_map/sdl_map.h"
#include "../sdl_panneaux/sdl_panneaux.h"
#include "../tile/tile.h"
#include "../technology/technology.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>

bool croissance_check(City* city);


// Chargement des sprites
SDL_Texture* load_sprite(SDL_Renderer* renderer, const char* filepath) {
    SDL_Surface* surface = SDL_LoadBMP(filepath);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Impossible de charger l'image %s : %s", filepath, SDL_GetError());
        return NULL;
    }
    
    // Rendre le fond rose fluo des sprites (255, 0, 255) transparent
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface); // On libère la surface qui ne sert plus
    
    return texture;
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
    int R = 60; 
    int h = (int)(R * 0.866f);

    // Centrage initial de la caméra sur la ville de départ !
    Position start_city = get_starting_city_pos(game->map);
    
    int cameraX = (start_city.x != -1) ? start_city.x * (2 * h) + ((start_city.y % 2) * h) : 0;
    int cameraY = (start_city.y != -1) ? start_city.y * (1.5f * R) : 0;

    //On calcule la taille maximale de la carte en pixels pour bloquer la caméra (qu'elle n'aille pas dans des énormes coordonnées)
    int maxWidth = game->map->length * (2*h);
    int maxHeight = game->map->height * (1.5f * R);

    // variables d'état du jeu
    Unit* selected_unit = NULL;
    char last_message[100] = "Bienvenue dans Civ PP2ix !";
    int show_arbre_tech = 0; // arbre techno initialement pas affiché
    bool show_guide_actions = false ; //fermé initialement
    int status_fin = 0; //ni victoire ni défaite initialement

    // Chargement de tous les sprites (SDL2_gfxPrimitives)
    SDL_Texture* tex_ville = load_sprite(renderer, "src/sprites/ville.bmp");
    SDL_Texture* tex_ville_mur = load_sprite(renderer, "src/sprites/ville_muraille.bmp"); 
    SDL_Texture* tex_guerrier = load_sprite(renderer, "src/sprites/guerrier.bmp");
    SDL_Texture* tex_colon = load_sprite(renderer, "src/sprites/colon.bmp");
    SDL_Texture* tex_barbare = load_sprite(renderer, "src/sprites/barbares.bmp");       
    SDL_Texture* tex_camp = load_sprite(renderer, "src/sprites/camp_barbares.bmp");
    //SDL_Texture* tex_bat_const = load_sprite(renderer, "src/sprites/batiment_construction.bmp");


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

                    // Sécurité
                    if (status_fin != 0) {
                        break; 
                    }

                    //arbre technologique (touche T)
                    if (event.key.keysym.sym == SDLK_t) {
                        show_arbre_tech = !show_arbre_tech;
                        snprintf(last_message, sizeof(last_message), "Menu arbre technologique");
                    }

                    // Choisir une technologie quand l'arbre technologique est ouvert
                    else if (show_arbre_tech) {
                        int tech_id = -1;

                        if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_8) {
                            tech_id = event.key.keysym.sym - SDLK_0;
                        } else if (event.key.keysym.sym >= SDLK_KP_1 && event.key.keysym.sym <= SDLK_KP_8) {
                            tech_id = event.key.keysym.sym - SDLK_KP_0;
                        }
                        if (tech_id != -1) {
                            if (set_active_research(game, tech_id) == 0) {
                                snprintf(last_message, sizeof(last_message), "Recherche lancee : %s",
                                    game->tech_tree->technologies[tech_id].name);
                            } else {
                                snprintf(last_message, sizeof(last_message), "Recherche impossible pour cette technologie.");
                            }
                        }
                    }

                    // Activer / Masquer le guide d'actions (Touche H)
                    if (event.key.keysym.sym == SDLK_h) {
                        show_guide_actions = !show_guide_actions;
                        if (show_guide_actions) {
                            snprintf(last_message, sizeof(last_message), "Affichage du guide d'aide active.");
                        } else {
                            snprintf(last_message, sizeof(last_message), "Guide masque. Appuyez sur [H] pour le revoir.");
                        }
                    }

                    // Passer le tour au clavier (touche F)
                    if (event.key.keysym.sym == SDLK_f && !show_arbre_tech && status_fin == 0) {
                        
                        // On applique le gros calcul du moteur (Production, Or, Bouffe, Projets ET Barbares)
                        int game_status = end_turn(game);

                        // On regarde si on a une condition de victoire ou défaite qui a été remplie
                        if (game_status != 0) {
                            status_fin = game_status; // Bloque le jeu et active l'écran de fin au prochain rendu
                        }

                        else {
                            // On passe officiellement au numéro de tour suivant
                            game->active_turn++;    
                            snprintf(last_message, sizeof(last_message), "Tour %d : Revenus percus. Vos unites ont recupere leurs PM.", game->active_turn);
                        }
                    }
                    
                    // Contrôle et déplacements des unités
                    if (position_actuelle.x != -1 && !show_arbre_tech) {
                        Tile* tuile_suivante = game->map->map[position_actuelle.y][position_actuelle.x];

                        // Touche M : Sélectionner ou Déplacer l'unité
                        if (event.key.keysym.sym == SDLK_m) {
                            if (selected_unit == NULL) {

                                if (tuile_suivante && tuile_suivante->unit) {
                                    selected_unit = tuile_suivante->unit;
                                    snprintf(last_message, sizeof(last_message), "Unite selectionnee. Cliquez sur la case cible + pressez [M]");
                                } 
                                
                                else {
                                    snprintf(last_message, sizeof(last_message), "Aucune unite sur cette case.");
                                }
                            } 

                            else {
                                // On tente de déplacer l'unité sélectionnée vers la case illuminée
                                MoveResult res = move_unit_step(game, selected_unit, position_actuelle);
                                get_move_message(res, last_message, sizeof(last_message));
                                selected_unit = NULL; // On relâche l'unité après l'action
                            }
                        }

                        // Touche V : Fonder une ville
                        if (event.key.keysym.sym == SDLK_v) {

                            if (tuile_suivante->unit && tuile_suivante->unit->type == 'c') {
                                colonize(game, tuile_suivante->unit);
                                snprintf(last_message, sizeof(last_message), "Ville fondee avec succes !");
                                selected_unit = NULL; 
                            } 
                            
                            else {
                                snprintf(last_message, sizeof(last_message), "Seul un Colon peut fonder une ville");
                            }
                        }

                    }

                    // Commandes de production des Villes
                    if (position_actuelle.x != -1 && !show_arbre_tech) {

                        City* city = find_city_at_position(game, position_actuelle);
                        if (city != NULL) {
                            
                            // Pour savoir si on veut produire quelque chose
                            bool touche_prod = (event.key.keysym.sym == SDLK_c || event.key.keysym.sym == SDLK_g || 
                                               (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_6) ||
                                               (event.key.keysym.sym >= SDLK_KP_1 && event.key.keysym.sym <= SDLK_KP_6));

                            if (touche_prod) {

                                // On vérifie qu'il n'y a pas d'autres projets en cours
                                if (city->project != NULL) {
                                    snprintf(last_message, sizeof(last_message), "ERREUR : La ville travaille deja sur un projet !");
                                } 

                                else {
                                    switch (event.key.keysym.sym) {

                                        // Création d'un Colon (touche C)
                                        case SDLK_c:
                                            start_project(city, 'c', position_actuelle);
                                            snprintf(last_message, sizeof(last_message), "SUCCES : Projet Colon planifie ! (50 pr)");
                                            break;

                                        // Création d'un guerrier (touche G)
                                        case SDLK_g:
                                            
                                            if (!is_unit_unlocked(game->tech_tree, 'g')) {
                                                snprintf(last_message, sizeof(last_message), "ERREUR : Technologie [Artisanat] requise pour le Guerrier !");
                                            }

                                            else if (!buildlist_contains(get_buildings_list(city), 'C')) {
                                                snprintf(last_message, sizeof(last_message), "ERREUR : Caserne requise dans cette ville !");
                                            } 
                                            
                                            else {
                                                start_project(city, 'g', position_actuelle);
                                                snprintf(last_message, sizeof(last_message), "SUCCES : Projet Guerrier planifie ! (40 pr)");
                                            }
                                            break;
                                        
                                        // Création d'un grenier (touche 1)
                                        case SDLK_1:
                                            start_project(city, 'G', position_actuelle);
                                            snprintf(last_message, sizeof(last_message), "SUCCES : Construction du Grenier planifiee ! (30 pr)");
                                            break;
                                        
                                        // Création d'un atelier (touche 2)
                                        case SDLK_2:
                                            start_project(city, 'A', position_actuelle);
                                            snprintf(last_message, sizeof(last_message), "SUCCES : Construction de l'Atelier planifiee ! (40 pr)");
                                            break;
                                        
                                        // Création d'une bibliothèque (touche 3)
                                        case SDLK_3: case SDLK_KP_3:

                                            if (!is_building_unlocked(game->tech_tree, 'B')) {
                                                snprintf(last_message, sizeof(last_message), "ERREUR : Technologie [Ecriture] requise !");
                                            } 
                                            
                                            else {
                                                start_project(city, 'B', position_actuelle);
                                                snprintf(last_message, sizeof(last_message), "SUCCES : Construction de la Bibliotheque planifiee ! (50 pr)");
                                            }
                                            break;

                                        // Création d'un marché (touche 4)
                                        case SDLK_4:
                                            if (!is_building_unlocked(game->tech_tree, 'M')) {
                                                snprintf(last_message, sizeof(last_message), "ERREUR : Technologie [Commerce] requise !");
                                            } 
                                            
                                            else {
                                                start_project(city, 'M', position_actuelle);
                                                snprintf(last_message, sizeof(last_message), "SUCCES : Construction du Marche planifiee ! (40 pr)");
                                            }
                                            break;

                                        // Création d'une caserne (touche 5)
                                        case SDLK_5:
                                            start_project(city, 'C', position_actuelle);
                                            snprintf(last_message, sizeof(last_message), "SUCCES : Construction de la Caserne planifiee ! (60 pr)");
                                            break;

                                        // Création d'une muraille (touche 6)
                                        case SDLK_6:

                                            if (!is_building_unlocked(game->tech_tree, 'R')) {
                                                snprintf(last_message, sizeof(last_message), "ERREUR : Technologie [Maconnerie] requise !");
                                            } 
                                            
                                            else {
                                                start_project(city, 'R', position_actuelle);
                                                snprintf(last_message, sizeof(last_message), "SUCCES : Construction de la Muraille planifiee ! (80 pr)");
                                            }
                                            break;
                                    }
                                }
                            }
                        }
                    }

                    
                
                    // On vérifie les limites avant de valider le mouvement
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

                    // Si panneau victoir/défaite actif :
                    if (status_fin != 0) {
                        // On récupère la taille dynamique pour calculer la zone exacte du bouton rouge QUITTER LE JEU
                        int current_w, current_h;

                        SDL_GetRendererOutputSize(renderer, &current_w, &current_h);
                        
                        int bx1 = (current_w - 260) / 2;
                        int by1 = ((current_h + 300) / 2) - 70; 
                        int bx2 = bx1 + 260;
                        int by2 = by1 + 40;

                        // Si le joueur clique sur le bouton rouge "QUITTER JEU"
                        if (mx >= bx1 && mx <= bx2 && my >= by1 && my <= by2) {
                            running = 0; // Coupe la boucle principale et ferme le jeu
                        }
                        break; // Intercepte le clic pour ne pas toucher la carte
                    }
                    
                    // gestion des clics sur les boutons des panneaux d'abord

                    //arbre tech
                    if (mx >= 20 && mx <= 150 && my >= 75 && my <= 105) {
                        show_arbre_tech = !show_arbre_tech;
                        break;
                    }

                    //guide d'actions / aide
                    else if (mx >= 170 && mx <= 300 && my >= 75 && my <= 105 && !show_arbre_tech) {
                            show_guide_actions = !show_guide_actions;
                            if (show_guide_actions) {
                                snprintf(last_message, sizeof(last_message), "Affichage du guide d'aide active.");
                            } else {
                                snprintf(last_message, sizeof(last_message), "Guide d'aide masque.");
                            }
                            break;
                    }

                    //fin de tour
                    else if (mx >= 20 && mx <= 300 && my >= 115 && my <= 145 && !show_arbre_tech) {

                        // Fin du tour
                        int game_status = end_turn(game);

                        // Verif si y'a victoire ou défaite
                        if (game_status != 0) {
                            status_fin = game_status; // Déclenche le panneau
                        } 

                        else {
                            game->active_turn++;
                            snprintf(last_message, sizeof(last_message), "Tour %d : Productions calculees.", game->active_turn);
                        }
                        break;
                    }

                    // clic sur le terrain (Seulement si l'arbre techno est fermé)
                    else if (!show_arbre_tech) {
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

    // Récupère la taille de fenêtre pour un affichage qui s'adapte en fct de la taille de la fenêtre
    int current_w, current_h;
    SDL_GetRendererOutputSize(renderer, &current_w, &current_h);

    //Dessin de la map avec la caméra
    draw_map_sdl(renderer, game, R, h, position_actuelle, cameraX, cameraY,
                tex_ville, tex_ville_mur, tex_guerrier, tex_colon, tex_camp, tex_barbare);

    //Dessin du tableau d'affichage global
    draw_panneau_global(renderer, game);
    
    //Affichage du panneau de guide d'actions
    if (show_guide_actions && !show_arbre_tech) {
        draw_panneau_guide_actions(renderer, game); 
    }

    //dessin panneau des messages 
    draw_panneau_message_action(renderer,last_message,current_w);

    //dessin du panneau qui donne les infos du biome
    draw_panneau_biome_flottant(renderer, game, position_actuelle, current_w);

    // Dessin du panneau de la tuile illuminee en bas, qui s'affiche dynamiquement
    draw_panneau_tuile_illuminee(renderer, game, position_actuelle, current_w, current_h, selected_unit);

    // Si l'arbre techno est ouvert, on l'affiche par-dessus tout le reste
    if (show_arbre_tech) {
        draw_panneau_arbre_tech(renderer, game, current_w, current_h);
    }

    // Si Victoire ou défaite, on affiche le panneau au dessus du reste
    if (status_fin != 0) {
        draw_panneau_fin_partie(renderer, game, status_fin, current_w, current_h);
    }

    SDL_RenderPresent(renderer);
    }

    //NETTOYAGE

    if(tex_ville) SDL_DestroyTexture(tex_ville);
    if(tex_ville_mur) SDL_DestroyTexture(tex_ville_mur);
    if(tex_guerrier) SDL_DestroyTexture(tex_guerrier);
    if(tex_colon) SDL_DestroyTexture(tex_colon);
    if(tex_barbare) SDL_DestroyTexture(tex_barbare); 
    if(tex_camp) SDL_DestroyTexture(tex_camp);
    //if(tex_bat_const) SDL_DestroyTexture(tex_bat_const);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
}
