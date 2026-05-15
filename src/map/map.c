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
            if (alea < 40) {      m->map[i][j] = create_tile(pos, 'P');} // 40% Plaine
            else if (alea < 50) { m->map[i][j] = create_tile(pos, 'F');} // 10% Forêt
            else if (alea < 60) { m->map[i][j] = create_tile(pos, 'M');} // 10% Montagne
            else if (alea < 70) { m->map[i][j] = create_tile(pos, 'E');} // 10% Eau
            else if (alea < 80) { m->map[i][j] = create_tile(pos, 'D');} // 10% Désert
            else{                 m->map[i][j] = create_tile(pos, 'T');} // 20% Toundra
        }
    }

    // 2. PLACEMENT DE LA VILLE INITIALE
    Position start_pos = {-1, -1};
    int ville_placee = 0; 
    
    while (ville_placee == 0) {
        int rx = rand() % width; //Méthode pas ouf de parcours aléatoire de la map pour trouver une plaine.
        int ry = rand() % height; // Mais il y a 40% de plaine donc ça devrait en trouver une rapidement
        
        if (m->map[ry][rx]->biome == 'P') { // Si c'est une Plaine
            m->map[ry][rx]->city_on = true; // On valide sur la tuile
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
                t->camp_on = true; // On valide sur la tuile
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
#define VIEW_RADIUS 10

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
/*
// Le champ de vision de la carte
#define VIEW_RADIUS 6 


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
                const char* fg = "\x1b[30m"; // Couleur du texte par défaut = noir
                
                if (tuile->city_on){
                    bg = COLOR_VILLE;
                    fg = "\x1b[31;1m"; // Texte en rouge et gras
                }
                else if (tuile->unit){
                    bg = COLOR_UNITE;
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
*/

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
