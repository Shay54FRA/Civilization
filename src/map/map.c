#include "map.h"
#include "../tile/tile.h"
#include <stdlib.h>
#include <stdio.h>

Map* create_map(int width, int height, int seed){

    srand(seed); //Decide de la génération aléatoire selon la seed donnée. (Permet de rejouer la partie)

    Map* m = malloc(sizeof(Map));
    m->height=height;
    m->length=width;
    m->map=malloc(sizeof(Tile**)*height);

    for(int i=0;i<height;i=i+1){
        m->map[i]=malloc(sizeof(Tile*)*width);

        for(int j=0;j<width;j=j+1){
            Position pos = {j, i}; // X = Colonnes (largeur), Y = Lignes (hauteur)
            //Initialisation biome
            int alea = rand() % 100;
            if (alea < 40) {
                m->map[i][j] = create_tile(pos, 'P'); // 40% de chance d'avoir une Plaine
            }
            else if (alea < 50) {
                m->map[i][j] = create_tile(pos, 'F'); // 10% de chance d'avoir une Forêt
            }
            else if (alea < 60) {
                m->map[i][j] = create_tile(pos, 'M'); // 10% de chance d'avoir une Montagne
            }
            else if (alea < 70) {
                m->map[i][j] = create_tile(pos, 'E'); // 10% de chance d'avoir de l'Eau
            }
            else if (alea < 80) {
                m->map[i][j] = create_tile(pos, 'D'); // 10% de chance d'avoir un Désert
            }
            else{
                m->map[i][j] = create_tile(pos, 'T'); // 20% de chance d'avoir une Toundra
            }
        }
    }
    return m;
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




void print_pos(Position pos) {
    printf("Position : (%d, %d)", pos.x, pos.y);
}

void print_tile(Tile* tile) {
    if (tile != NULL) {
        print_pos(tile->pos);
        printf(", Unité dessus : %d, Exploité : %d, Biome : %c\n", tile->unit != NULL, tile->city_on != NULL, tile->biome);
    }
}

// Le champ de vision de la carte
#define VIEW_RADIUS 6 

void print_map(Map* m, Position cursor) {
    if (m == NULL || m->map == NULL) return;
    system("clear"); //permet de clear le terminal

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
