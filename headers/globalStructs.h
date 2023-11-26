#include "../headers/vectorPawn.h"

#ifndef GLOBALSTRUCTS_H
#define GLOBALSTRUCTS_H

#define COLUMNS_COUNT 24
#define BOARD_COLUMN_HEIGHT 5
#define PAWNS_COUNT 15
#define MAX_PAWN_ON_COL 5
#define COLUMNGAP 3

#define STARTING_COLUMN_POINT 12

#define BLACK_PAWN  "\u2592\u2592"
#define WHITE_PAWN "\u2593\u2593"
#define SQUARE "\u25A0"

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
} s_board;

typedef struct  {
  int* dice;
  int diceSize;
  int isDoublet;
  int availableDiceMoves;
  int initialDiceValues[2];
} s_diceInfo;


typedef struct
{
  int initialDiceValueW;
  int initialDiceValueB;
  char turn;
  s_board board;
  s_pawn wPawns[PAWNS_COUNT];
  s_pawn bPawns[PAWNS_COUNT];
  s_pawn removePawns[2 * PAWNS_COUNT];
  s_diceInfo diceInfo;
} s_game;
#endif // GLOBALSTRUCTS_H
