//
// Created by jakub bot on 30/11/2023.
//
#include "../headers/globalStructs.h"
#include "../headers/bidirectionalList.h"

#ifndef BACKGAMMON2_RENDERGAMEUTILS_H
#define BACKGAMMON2_RENDERGAMEUTILS_H

void findColumnWithPawn(s_game game, int *currentActiveColumn, char action);
int findNextPossibleMove(s_game game, int currentActiveColumn, char action);
int validPawnToColumnMove(s_game *game, int nextColX);
int shouldUseBarPawn(s_game game);
void fillBList(struct Node **b_list, s_game *game);
void getInitialColumnsIds(vector_t_pawn *boardColumn, int col);
void updateDiceDublet(s_game *game, struct Node *b_list);
void updateDice(s_game *game, struct Node *b_list);
void clearGameState(s_game* game);

#endif //BACKGAMMON2_RENDERGAMEUTILS_H
