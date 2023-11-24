#include "../headers/vectorPawn.h"

#ifndef GLOBALSTRUCTS_H
#define GLOBALSTRUCTS_H

#define COLUMNS_COUNT 24
#define BOARD_COLUMN_HEIGHT 5
#define PAWNS_COUNT 15
#define MAX_PAWN_ON_COL 5
#define COLUMNGAP 3

typedef struct
{
  vector_t_pawn pawnIds;
  //    int pawnIds[MAX_PAWN_ON_COL];
  int colX;
  int colY;
  int isReversed;
} s_boardColumn;

typedef struct
{
  s_pawn pawnOnBar;
  s_boardColumn columns[COLUMNS_COUNT];
  s_boardColumn bar;
} s_board;

typedef struct
{
  int initialDiceValueW;
  int initialDiceValueB;
  char turn;
  s_board board;
  s_pawn wPawns[PAWNS_COUNT];
  s_pawn bPawns[PAWNS_COUNT];
  s_pawn removePawns[2 * PAWNS_COUNT];
  int rollDice[2];
} s_game;
#endif // GLOBALSTRUCTS_H
