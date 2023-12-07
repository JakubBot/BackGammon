#include <stdio.h>
#include "../headers/vectorPawn.h"

#ifndef GLOBALSTRUCTS_H
#define GLOBALSTRUCTS_H

#define COLUMNS_COUNT 24
#define BOARD_COLUMN_HEIGHT 5
#define PAWNS_COUNT 15
#define MAX_PAWN_ON_COL 5
#define COLUMNGAP 3
#define COLUMN_WIDTH 4

#define STARTING_COLUMN_POINT 12

#define BLACK_PAWN "\u2592\u2592"
#define WHITE_PAWN "\u2593\u2593"
#define SQUARE "\u25A0"

#define CURRENT_GAME "currentGame.txt"
#define SAVED_GAME "savedGame.txt"

#define BAR_COLUMN_HEIGHT ((MAX_PAWN_ON_COL * 2) + COLUMNGAP)

typedef struct
{
  vector_t_pawn pawnIds;
  int colX;
  int colY;
  int isReversed;
} s_boardColumn;

typedef struct
{
  vector_t_pawn pawnIds;
} s_bar;

typedef struct
{
  int isBarActive;
  s_boardColumn columns[COLUMNS_COUNT];
  s_bar bar;
  int sourceColumn;
  int targetColumn;
  int forcedTargetColumn;
  int pawnMoveToCourt;
} s_board;

typedef struct
{
  int dice[4];
  // int *dice;
  int diceSize;
  int isDoublet;
  int availableDiceMoves;
  int initialDiceValues[2];
} s_diceInfo;

typedef struct
{
  int winner;
  int points;
} s_endGame;

typedef struct
{
  int exitGameReview;
  int currentFilePosition;
  int fileLength;
} s_gameReview;

typedef struct
{
  char color;
  int whiteTurn;
  int moves;
  int moveArr[4];
  // int isDoublet;
  // int availableDiceMoves;
  // int allNormalMovesAvailable;
} s_diceContainer;
typedef struct
{
  int initialDiceValueW;
  int initialDiceValueB;
  char turn;
  s_board board;
  // s_pawn wPawns[PAWNS_COUNT];
  // s_pawn bPawns[PAWNS_COUNT];
  vector_t_pawn courtPawns;
  s_diceInfo diceInfo;
  int isPawnsHome;
  int removeFurthestPawn;

  FILE *file;
  int gameLoadedFromFile;
  s_endGame endGame;

  s_gameReview gameReview;

  int currentMenuBtnIndex;
} s_game;

#endif // GLOBALSTRUCTS_H
