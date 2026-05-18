#include "map.h"
#include "../tile/tile.h"
#include "../cli/cli.h"
#include <ncurses.h> // Indispensable
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void) {
    

    Map* ma_carte = create_map(10, 10, 42, 5);
    Position cursor = {0, 0}; // Position initiale de la caméra

    // Démarrage de l'interface (fonction provenant de cli.)
    init_ncurses_interface();


    print_map_cli(ma_carte, cursor);
    
    printw("\nAppuyez sur n'importe quelle touche pour quitter le test...");
    refresh(); // Très important pour afficher l'interface

    // Attends que j'appuies sur une touche quelconque avant de fermer la fen^tre
    getch();

    // On éteint l'interface proprement avant de quitter
    endwin();

    destroy_map(ma_carte);

    return 0;
}