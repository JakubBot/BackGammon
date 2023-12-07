#include "../headers/globalStructs.h"

#ifndef RENDERGAME_H
#define RENDERGAME_H


void renderGame(int loadFromFile);
void initGame();
void gameReview();
void renderBoard(s_game *game, WINDOW *gameWin, int selectColumn, int targetColumn, struct Node *b_list);
int setSourceColumn(s_game *game, WINDOW *gameWin, struct Node *b_list);

#endif //RENDERGAME_H
