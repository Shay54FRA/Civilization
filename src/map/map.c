#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "map.h"
#include "../tile/tile.h"
#include "../unit/unit.h"
#include <ncurses.h>

#define INF 1e7

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
#define VIEW_RADIUS_X 6
#define VIEW_RADIUS_Y 4

void print_map_cli(WINDOW* win,Map* m, Position cursor) {
    if (m == NULL || m->map == NULL) return;

    int start_x = 0;
    int end_x = 0;
    int start_y = 0;
    int end_y = 0;

    
    if(cursor.y < VIEW_RADIUS_Y){
        start_y = 0;
        end_y = 2*VIEW_RADIUS_Y;
    }
    else if(cursor.y > (m->height-VIEW_RADIUS_Y) ){
        start_y = m->height-2*VIEW_RADIUS_Y-1 ;;
        end_y = m->height - 1;
    }
    else{ //(cursor.y >= VIEW_RADIUS || cursor.y <= m->height-VIEW_RADIUS)
        start_y = cursor.y - VIEW_RADIUS_Y;
        end_y = cursor.y + VIEW_RADIUS_Y;
    }

    if(cursor.x<VIEW_RADIUS_X){
        start_x = 0;
        end_x = 2*VIEW_RADIUS_X;
    }
    else if(cursor.x > (m->length-VIEW_RADIUS_X) ){
        start_x = m->length-2*VIEW_RADIUS_X-1 ;
        end_x = m->length - 1;
    }
    else{ //(cursor.x >= VIEW_RADIUS || cursor.x <= m->length-VIEW_RADIUS)
        start_x = cursor.x - VIEW_RADIUS_X;
        end_x = cursor.x + VIEW_RADIUS_X;
    }

    for (int y = start_y; y <= end_y; y++) {
        
        // Pour faire de grosses cases, on dessine sur 3 lignes
        for (int line = 0; line < 3; line++) {
            
            // Pour faire un effet hexagone, on décale les lignes impaires
            if (y % 2 != 0) {
                wprintw(win,"     "); // On décale de 5 espaces car nos cases font 9 de large + 1 espace de séparation
            }

            for (int x = start_x; x <= end_x; x++) {
                
                // Si la caméra regarde dans le vide (hors carte)
                if (x < 0 || x >= m->length || y < 0 || y >= m->height) {
                    wprintw(win,"");
                    continue;
                }

                Tile* tuile = m->map[y][x];
                
                // 1. Choix du symbole à afficher sur la case
                char symbol = ' ';
                // symbol = tuile->biome; //J'ai enlevé la lettre du biome
                if (tuile->city_on) symbol = 'V';
                else if (tuile->unit) symbol = 'U';
                else if (tuile->camp_on) symbol = 'C';
                else if (tuile->barb_on) symbol = 'B';

                // 2. Gestion des couleurs ncurses (couleurs définies dans mpa.h)
                int current_color = COLOR_PLAINE;                
                if (tuile->city_on){ current_color = COLOR_VILLE;}
                else {
                    switch(tuile->biome) {
                        case 'E': current_color = COLOR_EAU; break;
                        case 'P': current_color = COLOR_PLAINE; break;
                        case 'F': current_color = COLOR_FORET; break;
                        case 'M': current_color = COLOR_MONTAGNE; break;
                        case 'D': current_color = COLOR_DESERT; break;
                        case 'T': current_color = COLOR_TOUNDRA; break;
                    }
                }

                // 3. DESSIN D'UNE CASE (j'ai dessiné les cases sur une hauteur de 3 lignes et une largeur de 9 caractères)

                if (line == 0 || line == 2) { // --- Lignes du HAUT et du BAS d'une case---

                    if (cursor.x == x && cursor.y == y) { //Case actuelle encadrée en rouge
                        wattron(win,COLOR_PAIR(COLOR_CURSEUR)); // wattron change la couleur d'écriture
                        wprintw(win,"+-------+");
                        wattroff(win,COLOR_PAIR(COLOR_CURSEUR));
                        wprintw(win," ");
                    }

                    else {
                        // Bloc de couleur uni
                        wattron(win,COLOR_PAIR(current_color));
                        wprintw(win,"         ");
                        wattroff(win,COLOR_PAIR(current_color));
                        wprintw(win," ");
                    }
                }
                
                
                
                else if (line == 1) { // --- Ligne du MILIEU (avec la lettre) ---
                    if (cursor.x == x && cursor.y == y) { 
                        wattron(win,COLOR_PAIR(COLOR_CURSEUR)); wprintw(win,"|"); wattroff(win,COLOR_PAIR(COLOR_CURSEUR)); // Bordure rouge '|' et lettre au centre
                        wattron(win,COLOR_PAIR(current_color)); wprintw(win,"   %c   ", symbol); wattroff(win,COLOR_PAIR(current_color));
                        wattron(win,COLOR_PAIR(COLOR_CURSEUR)); wprintw(win,"|"); wattroff(win,COLOR_PAIR(COLOR_CURSEUR));
                        wprintw(win," ");
                    }
                    else {
                        wattron(win,COLOR_PAIR(current_color));
                        wprintw(win,"    %c    ", symbol); // 4 espaces, char, 4 espaces, affichage sur 9 cases de large
                        wattroff(win,COLOR_PAIR(current_color));
                        wprintw(win," ");
                    }
                }
            }
            wprintw(win,"\n"); // On passe à la ligne suivante du terminal
        }
        wprintw(win,"\n"); // On ajoute un espace vertical entre chaque rangée de cases
    }
    // wprintw(win,"=== CAMERA - POSITION : (%d, %d) ===\n\n", cursor.x, cursor.y);
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

Position* create_position(int x, int y) {
    Position* pos = malloc(sizeof(Position));
    if (pos == NULL) return NULL;
    pos->x = x;
    pos->y = y;
    return pos;
}

void destroy_position(Position* pos) {
    if (pos != NULL) {
        free(pos);
    }
}

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

//Dijkstra qui permet de renvoyer la position sur laquel se déplacer
Position dijkstra(Map* map, Position pos_start, Position pos_end) {
    int width = map->length;
    int height = map->height;
    int dist[width][height];
    bool visited[width][height];
    Position parent[width][height];

    // Initialisation de la grille
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            dist[x][y] = INF;
            visited[x][y] = false;
            parent[x][y] = (Position){-1, -1};
        }
    }

    dist[pos_start.x][pos_start.y] = 0;
    for (int count = 0; count < width * height; count++) {
        // 1. Trouver le nœud non visité avec la distance minimale
        int min_dist = INF;
        Position u = {-1, -1};

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                if (!visited[x][y] && dist[x][y] < min_dist) {
                    min_dist = dist[x][y];
                    u = (Position){x, y};
                }
            }
        }

        // Si on ne trouve plus de nœud accessible ou si on a atteint la destination
        if (u.x == -1 || (u.x == pos_end.x && u.y == pos_end.y)) {
            break;
        }

        visited[u.x][u.y] = true;
        // 2. Mettre à jour les distances des 6 voisins hexagonaux
        TileList* every_voisins = get_tiles_at_range(map, u, 1);
        TileList* voisins = every_voisins;
        while (voisins != NULL) {
            Tile* tile = voisins->data;
            if (tile != NULL) {
                Position pos_voisin = tile->pos;
                int v_x = pos_voisin.x;
                int v_y = pos_voisin.y;
                if (!visited[v_x][v_y]) {
                    int weight = get_terrain_cost(tile->biome);
                    if (tile->barb_on != NULL || tile->camp_on) {
                        weight = 999;
                    }
                
                    if (dist[u.x][u.y] + weight < dist[v_x][v_y]) {
                        dist[v_x][v_y] = dist[u.x][u.y] + weight;
                        parent[v_x][v_y] = u;
                    }
                }
            }
            voisins = voisins->next;
        }
        destroy_tilelist(every_voisins);
    }

    Position next_step = pos_start;

    // On vérifie d'abord si la destination a bien un parent valide (chemin trouvé)
    if (parent[pos_end.x][pos_end.y].x != -1) {
        Position curr = pos_end;
        // On remonte jusqu'à trouver la case juste après pos_start
        while (curr.x != -1 && curr.y != -1) {
            Position p = parent[curr.x][curr.y];
            if (p.x == pos_start.x && p.y == pos_start.y) {
                next_step = curr;
                break;
            }
            curr = p;
        }
    }

    return next_step;
}

void reset_exploitation(Map* map) {
    if (map == NULL) return;
    for (int x = 0; x < map->length; x++) {
        for (int y = 0; y < map->height; y++) {
            Position pos = {x,y};
            Tile* tile = get_tile(map, pos);
            if (tile != NULL) tile->exploited = false;
        }
    }
}

void mark_exploited_tiles(TileList* tile_list) {
    TileList* to_check = tile_list;
    while(to_check != NULL) {
        Tile* tile = to_check->data;
        if (tile != NULL) {
            tile->exploited = true;
        }
        to_check = to_check->next;
    }
}

TileList* get_tiles_at_range(Map* map, Position pos, int range) {
    if (map != NULL) {
        TileList* rep = create_tilelist(NULL);
        for (int x = pos.x - range; x <= pos.x + range; x++) {
            for (int y = pos.y - range; y <= pos.y + range; y++) {
                if (x >= 0 && x < map->length && y >= 0 && y < map->height) {
                    Tile* new_tile = map->map[y][x];
                    Position new_pos = {x, y};
                    if (get_distance(pos, new_pos) == range) {
                        append_tilelist(rep, new_tile);
                    }
                }
            }
        }
        return rep;
    }
    return NULL;
}

TileList* get_exploited_tiles(Map* map, Tile* tuile, int range) {
    Position pos = tuile->pos;
    if (map != NULL) {
        TileList* all_tiles = get_tiles_at_range(map, pos, range);
        TileList* rep = create_tilelist(NULL);
        TileList* current = all_tiles;
        while (current != NULL) {
            Tile* t = current->data;
            if (t != NULL && !t->exploited) {
                append_tilelist(rep, t);
            }
            current = current->next;
        }
        destroy_tilelist(all_tiles);
        return rep;
    }
    return NULL;
}
