#include "ncurses.h"
#include "../headers/globalStructs.h"
#ifndef UTILS_H
#define UTILS_H

#define COLOR_COLUMN 20
#define POSSIBLE_COLUMN_MOVE 21
#define FORCED_COLUMN_MOVE 22

#define COLOR_COLUMN_ID 2
#define POSSIBLE_COLUMN_MOVE_ID 3
#define FORCED_COLUMN_MOVE_ID 4

#define BOARD_WIDTH 52
#define BOARD_HEIGHT 14

void clrButtonPrints(int posY, int nLines);

void changeTurn(s_game *game);

s_boardColumn findColumnBasedOnColX(s_game game, int currentActiveColumn);
vector_t_pawn* findColumnPawnIds(s_game *game, int currentActiveColumn);
int allPawnsHome(s_game game);

int max(int a, int b);
int min(int a, int b);
int isBarActive(vector_t_pawn barPawnIds, s_game game);
int isWhiteTurn(s_game game);
char getTurn(s_game game);
int getPawnBarCount(s_game game);
int findPawnIndex(vector_t_pawn pawns, char turn);

#endif // UTILS_H
