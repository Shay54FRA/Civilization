#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "map.h"
#include "../tile/tile.h"

Map* create_map(int width, int height, int seed, int nb_camps){ //Ajout des camps de barbares et ville de départ

    srand(seed); //Decide de la génération aléatoire selon la seed donnée. (Permet de rejouer la partie)

    Map* m = malloc(sizeof(Map));
    m->height=height;
    m->length=width;
    m->map=malloc(sizeof(Tile**)*height);

    // 1.BIOMES
    for(int i=0;i<height;i=i+1){
        m->map[i]=malloc(sizeof(Tile*)*width);

        for(int j=0;j<width;j=j+1){
            Position pos = {j, i}; // X = Colonnes (largeur), Y = Lignes (hauteur)
            //Initialisation biome
            int alea = rand() % 100;
            if (alea < 19) {      m->map[i][j] = create_tile(pos, 'P');} // 19% Plaine
            else if (alea < 38) { m->map[i][j] = create_tile(pos, 'F');} // 16.5% Forêt
            else if (alea < 54.5) { m->map[i][j] = create_tile(pos, 'M');} // 16.5% Montagne
            else if (alea < 69.5) { m->map[i][j] = create_tile(pos, 'E');} // 15% Eau
            else if (alea < 86) { m->map[i][j] = create_tile(pos, 'D');} // 16.5% Désert
            else{                 m->map[i][j] = create_tile(pos, 'T');} // 16.5% Toundra
        }
    }

    smooth_map(m); // 1er passage
    smooth_map(m); // 2eme
    smooth_map(m); // 3eme
    smooth_map(m); // 4e
    smooth_map(m); // 5e
    smooth_map(m); // 6e


    // 2. PLACEMENT DE LA VILLE INITIALE
    Position start_pos = {-1, -1};
    int ville_placee = 0; 
    
    while (ville_placee == 0) {
        int rx = rand() % width; //Méthode pas ouf de parcours aléatoire de la map pour trouver une plaine mais ça passe.
        int ry = rand() % height; //
        
        if (m->map[ry][rx]->biome == 'P') { // Si c'est une Plaine
            m->map[ry][rx]->city_on = true;
            start_pos.x = rx;
            start_pos.y = ry;               // On garde la pos en mémoire car les barbares doivent être assez éloignés de la ville de départ
            ville_placee = 1;               
        }
    }

    // 3. PLACEMENT DES CAMPS BARBARES
    int camps_places = 0;
    while (camps_places < nb_camps) {
        int rx = rand() % width;
        int ry = rand() % height;
        Tile* t = m->map[ry][rx];
        Position p = {rx, ry};
        
        // Règle 1: Pas d'eau, pas de ville, pas déjà un camp
        if (t->biome != 'E' && !t->city_on && !t->camp_on) {
            // Règle 2: Au moins 5 tuiles de distance (Tchebychev) de la ville de départ
            if (get_distance(start_pos, p) >= 5) {
                t->camp_on = true;
                camps_places++;
            }
        }
    }

    return m;
}

Position get_starting_city_pos(Map* map) {
    if (map == NULL) return (Position){-1, -1};

    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->length; j++) {
            if (map->map[i][j]->city_on) {
                return map->map[i][j]->pos;
            }
        }
    }
    return (Position){-1, -1}; // Sécurité si aucune ville n'est trouvée
}

// Le champ de vision de la carte
#define VIEW_RADIUS 8

void print_map_cli(Map* m, Position cursor) {
    if (m == NULL || m->map == NULL) return;
    

    int start_y = cursor.y - VIEW_RADIUS;
    int end_y = cursor.y + VIEW_RADIUS;
    int start_x = cursor.x - VIEW_RADIUS;
    int end_x = cursor.x + VIEW_RADIUS;

    for (int y = start_y; y <= end_y; y++) {
        
        // Pour faire de grosses cases, on dessine sur 3 lignes
        for (int line = 0; line < 3; line++) {
            
            // Pour faire un effet hexagone, on décale les lignes impaires
            if (y % 2 != 0) {
                printf("     "); // On décale de 5 espaces car nos cases font 9 de large + 1 espace de séparation
            }

            for (int x = start_x; x <= end_x; x++) {
                
                // Si la caméra regarde dans le vide (hors carte)
                if (x < 0 || x >= m->length || y < 0 || y >= m->height) {
                    printf("     "); // 5 espaces
                    continue;
                }

                Tile* tuile = m->map[y][x];
                
                // 1. Choix du symbole à afficher sur la case
                char symbol = ' ';
                // symbol = tuile->biome; //J'ai enlevé la lettre du biome
                if (tuile->city_on) symbol = 'V';
                else if (tuile->unit) symbol = 'U';
                else if (tuile->camp_on) symbol = 'C';

                // 2. Gestion des couleurs des cases (couleurs définies dans map.h)

                    // '\x1b['   : début commande de style (couleur, police...)
                    // '31'      : texte en rouge
                    // ';1'      : texte en gras
                    // 'm'       : fin ordre de style
                    // '\x1b[30m' : permet de reset le style, je le mets à chaque fin de printf par sécurité ( COLOR RESET = "\x1b[30m" )

                const char* bg = ""; // Background color
                const char* fg = "\x1b[30;1m"; // Couleur du texte par défaut noir et gras
                
                if (tuile->city_on){
                    bg = COLOR_VILLE;
                    fg = "\x1b[31;1m"; // Texte en rouge et gras
                }

                else {
                    switch(tuile->biome) {
                        case 'E': bg = BG_EAU; break;
                        case 'P': bg = BG_PLAINE; break;
                        case 'F': bg = BG_FORET; break;
                        case 'M': bg = BG_MONTAGNE; break;
                        case 'D': bg = BG_DESERT; break;
                        case 'T': bg = BG_TOUNDRA; break;
                    }
                }

                // 3. DESSIN D'UNE CASE (j'ai dessiné les cases sur une hauteur de 3 lignes et une largeur de 9 caractères)

                if (line == 0 || line == 2) { // --- Lignes du HAUT et du BAS d'une case---

                    if (cursor.x == x && cursor.y == y) { //Case actuelle encadrée en rouge
                        printf("%s\x1b[31;1m+-------+%s ", bg,COLOR_RESET);
                    }

                    else {
                        // Bloc de couleur uni
                        printf("%s         %s ", bg,COLOR_RESET);
                    }
                }
                
                
                
                else if (line == 1) { // --- Ligne du MILIEU (avec la lettre) ---
                    if (cursor.x == x && cursor.y == y) { 
                        printf("%s\x1b[31;1m|%s   %c   \x1b[31;1m|%s ", bg, fg, symbol,COLOR_RESET); // Bordure rouge '|' et lettre au centre
                    }
                    else {
                        printf("%s%s    %c    %s ", bg, fg, symbol,COLOR_RESET); // Affichage case sur 9 de large
                    }
                }
            }
            printf("\n"); // On passe à la ligne suivante du terminal
        }
        printf("\n"); // On ajoute un espace vertical entre chaque rangée de cases
    }
    printf("=== CAMERA - POSITION : (%d, %d) ===\n\n", cursor.x, cursor.y);
}

char int_to_biome(int k){
    switch (k)
    {
    case 0: return 'E';
    case 1: return 'P';
    case 2: return 'F';
    case 3: return 'M';
    case 4: return 'D';
    case 5: return 'T';
    default: return 'P'; //J'évite les warning "non-void function does not return a value in all control paths"
    }
}

int biome_to_int(char k){
    switch (k)
    {
    case 'E': return 0;
    case 'P': return 1;
    case 'F': return 2;
    case 'M': return 3;
    case 'D': return 4;
    case 'T': return 5;
    default: return 1; //J'évite les warning "non-void function does not return a value in all control paths"
    }
}


void smooth_map(Map* m) { // Principe d'automate cellulaire très simple
    if (m == NULL) return;

    // 1. On crée une grille temporaire pour stocker les nouveaux biomes
    char temp_biomes[m->height][m->length];

    // 2. On parcourt toute la carte
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->length; x++) {

            // On a 6 biomes : P,F,M,E,D,T
            int counts[6] = {0}; // Tableau de compteurs des 6 biomes possibles
            
            // 3. On inspecte les 8 voisins (et la case elle-même)
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    
                    // On vérifie qu'on ne sort pas des limites de la carte
                    if (nx >= 0 && nx < m->length && ny >= 0 && ny < m->height) {
                        char biome_voisin = m->map[ny][nx]->biome;
                        counts[biome_to_int(biome_voisin)]++;
                        }
                    }
                }

            // 4. On cherche quel est le biome majoritaire autour de cette case
            char biome_majoritaire = m->map[y][x]->biome; // Par défaut, on garde le biome actuel
            int max_count = 0;

            for (int i = 0; i < 6; i++) {
                if (counts[i] > max_count) {
                    max_count = counts[i];
                    biome_majoritaire = int_to_biome(i);
                }
            }

            // On sauvegarde le résultat dans notre grille temporaire
            temp_biomes[y][x] = biome_majoritaire;
        }
    }

    // 5. Une fois que toute la carte a été calculée, on applique les modifications
    for (int y = 0; y < m->height; y++) {
        for (int x = 0; x < m->length; x++) {
            m->map[y][x]->biome = temp_biomes[y][x];
        }
    }
}

void destroy_map(Map* m) {
    if (m != NULL) {
        for (int i = 0; i < m->height; i++) {
            for(int j=0; j< m->length;j++){
                free(m->map[i][j]);
            }
            free(m->map[i]); 
        }

        free(m->map);
        
        free(m);
    }
}



/*
void print_pos(Position pos) {
    printf("Position : (%d, %d)", pos.x, pos.y);
}
*/

int get_distance(Position pos1, Position pos2){ //Distance de Tchebychev
    //On convertit les points dans un système de coordonnées approprié
    int q1 = pos1.x - (pos1.y + (pos1.y & 0)) / 2;
    int r1 = pos1.y;
    int s1 = -(q1 + r1);

    int q2 = pos2.x - (pos2.y + (pos2.y & 0)) / 2;
    int r2 = pos2.y;
    int s2 = -(q2 + r2);

    return (abs(q1 - q2) + abs(r1 - r2) + abs(s1 - s2)) / 2;
}

void reset_exploitation(Map* map) {
    if (map == NULL) return;
    for (int x = 0; x < map->length; x++) {
        for (int y = 0; y < map->height; y++) {
            Position pos = {x,y};
            Tile* tile = get_tile(map, pos);
            tile->exploited = false;
        }
    }
}

TileList* get_exploited_tiles(Map* map, Tile* tuile, int range) {
    Position pos = tuile->pos;
    if (map != NULL) {
        TileList* rep = create_tilelist(NULL);
        for (int x = pos.x-range; x<=pos.x+range; x++) {
            for (int y = pos.y-range; y<=pos.y+range; y++) {
                if (x >= 0 && x < map->length && y >= 0 && y < map->height) {
                    Tile* new_tile = map->map[y][x];
                    Position new_pos = {x,y};
                    if (get_distance(pos, new_pos) == range && !(new_tile->exploited)) {
                        new_tile->exploited = true;
                        append_tilelist(rep, new_tile);
                    }
                }
            }
        }
        return rep;
    }
    return NULL;
}
