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
int getNextMoveCalculation(int firstVal, int secondVal, int whiteTurn);
int makeMove(struct Node **b_list, char action);
int isValidNextMove(int move);
int getClrPair(int colX, int currentActiveColumn, int forcedTargetColumn);
void showGameReviewOptions(s_game *game, WINDOW *gameWin);
void initializeBoardData(s_game *game);
void initializeGameReview(s_game *game);
void initializeFile(s_game *game);
void hideMenu();
void handleSourceTargetMove(s_game *game, WINDOW *gameWin, struct Node *b_list);
void showInvalidMenu(s_game *game);
void printBottomColAdditionalPawns(int offset_y, int offset_x, int x, int labelRowId, int colCount, int verticalOffset);
void printUpperColAdditionalPawns(int offset_y, int offset_x, int x, int labelRowId, int col);
int handleBoardMoveRight(s_game *game, int *currentActiveColumn, struct Node **b_list, int *skippedColumns);
int handleBoardMoveLeft(s_game game, int *currentActiveColumn);
struct Node *getListElement(char action, struct Node **b_list, int *currentActiveColumn, int *skippedColumns);
int handlePawnToHome(s_game *game, int move, int *currentActiveColumn, char action, struct Node **b_list);


#endif //BACKGAMMON2_RENDERGAMEUTILS_H
