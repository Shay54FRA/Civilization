#ifndef CLI
#define CLI

typedef struct _Game Game;
typedef struct _Map Map;
typedef struct _Position Position;

void run_game_cli(Game* game);
void print_map_cli(Map* m, Position cursor);
void print_pos(Position pos);

#endif
