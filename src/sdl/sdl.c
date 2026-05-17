#include "../game/game.h"
#include "../configuration/configuration.h"
#include "../building/building.h"
#include "../unit/unit.h"
#include "../sdl_map/sdl_map.h"
#include "../sdl_panneaux/sdl_panneaux.h"
#include "../tile/tile.h"
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
    
    // Rendre le fond rose fluo (255, 0, 255) transparent
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

    // Chargement de tous les SDL2_gfxPrimitives
    SDL_Texture* tex_ville = load_sprite(renderer, "src/sprites/ville.bmp");
    SDL_Texture* tex_ville_mur = load_sprite(renderer, "src/sprites/ville_muraille.bmp"); 
    SDL_Texture* tex_guerrier = load_sprite(renderer, "src/sprites/guerrier.bmp");
    SDL_Texture* tex_colon = load_sprite(renderer, "src/sprites/colon.bmp");
    SDL_Texture* tex_barbare = load_sprite(renderer, "src/sprites/barbares.bmp");       
    SDL_Texture* tex_camp = load_sprite(renderer, "src/sprites/camp_barbares.bmp");
    SDL_Texture* tex_bat_const = load_sprite(renderer, "src/sprites/batiment_construction.bmp");


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
                        // On termine le tour actuel
                        end_turn(game);

                        // On initialise le nouveau tour (Calcule l'or, science, nourriture, projets, croissance et famine)
                        start_turn(game);

                        // Maintenance de l'interface graphique
                        reset_all_pm(game->unitList); // Réinitialise les mouvements du joueur pour le nouveau tour
                        game->active_turn++;          // Passage officiel au tour suivant

                        // 3. Vérification des conditions de Victoire / Défaite (Comme dans le CLI !)
                        int game_status = end_game(game);
                        if (game_status != 0) {
                            if (game_status == 1) {
                                snprintf(last_message, sizeof(last_message), "VICTOIRE TERRITORIALE ! (10+ villes)");
                            } 
                            
                            else if (game_status == 2) {
                                snprintf(last_message, sizeof(last_message), "VICTOIRE TECHNOLOGIQUE ! (Arbre complet)");
                            } 
                            
                            else if (game_status == 3) {
                                snprintf(last_message, sizeof(last_message), "DEFAITE ! Score final : %d", game_score(game));
                            }
                        } 
                        
                        else {
                            snprintf(last_message, sizeof(last_message), "Tour %d : Revenus percus et projets mis a jour.", game->active_turn);
                        }
                    }

                    // Unités et Ville
                    if (position_actuelle.x != -1 && !show_arbre_tech) {
                        Tile* tuile_suivante = game->map->map[position_actuelle.y][position_actuelle.x];

                        // Selectionner ou Déplacer une unité (Touche M)
                        if (event.key.keysym.sym == SDLK_m) {

                            if (selected_unit == NULL) {

                                if (tuile_suivante && tuile_suivante->unit) {
                                    selected_unit = tuile_suivante->unit;
                                    snprintf(last_message, sizeof(last_message), "Unite selectionnee, cliquez sur la case cible + la touche M");
                                }

                                else {
                                    snprintf(last_message, sizeof(last_message), "Aucune unite sur cette case");
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

                    // Commande [G] : Entraîner un Guerrier (Coût: 40)
                    if (event.key.keysym.sym == SDLK_g && !show_arbre_tech) {
                        if (position_actuelle.x != -1) {
                            City* city = find_city_at_position(game, position_actuelle);
                            if (city != NULL) {
                                // RÈGLE DU SUJET : Vérification de la présence d'une Caserne ('C')
                                if (!buildlist_contains(get_buildings_list(city), 'C')) {
                                    snprintf(last_message, sizeof(last_message), "ERREUR : Caserne requise dans cette ville pour le Guerrier !");
                                } 
                                // Tente de lancer le projet via la fonction de ton pote
                                else if (start_project(city, position_actuelle, 'g')) {
                                    snprintf(last_message, sizeof(last_message), "SUCCES : Projet Guerrier planifie ! (Cout: 40 pr)");
                                } else {
                                    snprintf(last_message, sizeof(last_message), "ERREUR : Un projet est deja en cours dans cette ville !");
                                }
                            } else {
                                snprintf(last_message, sizeof(last_message), "ERREUR : La case selectionnee n'est pas un centre urbain !");
                            }
                        } else {
                            snprintf(last_message, sizeof(last_message), "ERREUR : Cliquez sur une ville avant d'appuyer sur [G].");
                        }
                    }

                    // Commande [C] : Bâtir un Colon (Coût: 50)
                    if (event.key.keysym.sym == SDLK_c && !show_arbre_tech) {
                        if (position_actuelle.x != -1) {
                            City* city = find_city_at_position(game, position_actuelle);
                            if (city != NULL) {
                                // Le colon est disponible sans prérequis au départ
                                if (start_project(city, position_actuelle, 'c')) {
                                    snprintf(last_message, sizeof(last_message), "SUCCES : Projet Colon planifie ! (Cout: 50 pr)");
                                } else {
                                    snprintf(last_message, sizeof(last_message), "ERREUR : Un projet est deja en cours dans cette ville !");
                                }
                            } else {
                                snprintf(last_message, sizeof(last_message), "ERREUR : Pas de ville sur cette case !");
                            }
                        } else {
                            snprintf(last_message, sizeof(last_message), "ERREUR : Cliquez sur une ville avant d'appuyer sur [C].");
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

                    //arbre tech
                    if (mx >= 20 && mx <= 150 && my >= 75 && my <= 105) {
                        show_arbre_tech = !show_arbre_tech;
                        break;
                    }

                    //fin de tour
                    if (mx >= 170 && mx <= 300 && my >= 75 && my <= 105 && !show_arbre_tech) {
                        // On termine le tour actuel
                        end_turn(game);

                        // On initialise le nouveau tour (Calcule l'or, science, nourriture, projets, croissance et famine)
                        start_turn(game);

                        // Maintenance de l'interface graphique
                        reset_all_pm(game->unitList); // Réinitialise les mouvements du joueur pour le nouveau tour
                        game->active_turn++;          // Passage officiel au tour suivant

                        snprintf(last_message, sizeof(last_message), "Tour %d : Revenus percus, production allouee et tuiles rafraichies !", game->active_turn);
                    }

                    // clic sur le terrain (Seulement si l'arbre techno est fermé)
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
    draw_map_sdl(renderer, game, R, h, position_actuelle, cameraX, cameraY,
                tex_ville, tex_ville_mur, tex_guerrier, tex_colon, tex_camp, tex_barbare, tex_bat_const);

    //Dessin du tableau d'affichage global
    draw_panneau_global(renderer, game);
    
    //Affichage du panneau de guide d'actions pour la ville uniquement si on clique sur une ville
    if (position_actuelle.x != -1 && position_actuelle.y != -1) {
            City* selected_city = find_city_at_position(game, position_actuelle);
            if (selected_city != NULL) {
                draw_panneau_guide_actions(renderer, game); 
            }
        }

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

    if(tex_ville) SDL_DestroyTexture(tex_ville);
    if(tex_ville_mur) SDL_DestroyTexture(tex_ville_mur);
    if(tex_guerrier) SDL_DestroyTexture(tex_guerrier);
    if(tex_colon) SDL_DestroyTexture(tex_colon);
    if(tex_barbare) SDL_DestroyTexture(tex_barbare); 
    if(tex_camp) SDL_DestroyTexture(tex_camp);
    if(tex_bat_const) SDL_DestroyTexture(tex_bat_const);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
}
