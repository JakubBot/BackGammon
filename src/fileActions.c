#include "stdlib.h"
#include "../headers/globalStructs.h"

void saveCurrentState(s_game game)
{
  FILE *file = game.file;

  fprintf(file, "%d %d %c", game.initialDiceValueW, game.initialDiceValueB, game.turn);

  for (int i = 0; i < COLUMNS_COUNT; ++i)
  {
    vector_t_pawn *pawn = &game.board.columns[i].pawnIds;
    fprintf(file, " %d", pawn->count);
    for (int j = 0; j < pawn->count; ++j)
    {
      fprintf(file, " %d %c", pawn->ptr[j].id, pawn->ptr[j].color);
    }
  }

  vector_t_pawn *barPawn = &game.board.bar.pawnIds;
  fprintf(file, " %d", barPawn->count);
  for (int i = 0; i < barPawn->count; ++i)
  {
    fprintf(file, " %d %c", barPawn->ptr[i].id, barPawn->ptr[i].color);
  }

  fprintf(file, " %d %d %d", game.board.isBarActive, game.board.sourceColumn, game.board.targetColumn);

  vector_t_pawn *courtPawns = &game.courtPawns;
  fprintf(file, " %d", courtPawns->count);
  for (int i = 0; i < courtPawns->count; ++i)
  {
    fprintf(file, " %d %c", courtPawns->ptr[i].id, courtPawns->ptr[i].color);
  }
  // fprintf(file, " %d", game.diceInfo.diceSize);
  // for (int i = 0; i < game.diceInfo.diceSize; ++i)
  // {
  //   fprintf(file, " %d", game.diceInfo.dice[i]);
  // }

  // fprintf(file, " %d %d %d %d", game.diceInfo.isDoublet, game.diceInfo.availableDiceMoves,
  //         game.diceInfo.initialDiceValues[0], game.diceInfo.initialDiceValues[1]);

  fprintf(file, "\n");
}

void updateGameFile(int copyToCurrentFile, s_game game)
{
  char *source = copyToCurrentFile ? SAVED_GAME : CURRENT_GAME;
  char *target = copyToCurrentFile ? CURRENT_GAME : SAVED_GAME;
  FILE *sourceFile = NULL;
  FILE *destinationFile = NULL;
  // load from saved file to current
  //
  if (copyToCurrentFile == 0 && game.gameLoadedFromFile)
  {
    sourceFile = fopen(source, "r");
    destinationFile = fopen(target, "w");
  }
  else
  {
    sourceFile = fopen(source, "r");
    destinationFile = fopen(target, "w");
  }

  if (sourceFile == NULL || destinationFile == NULL)
  {
    return;
  }

  int character;

  while ((character = fgetc(sourceFile)) != EOF)
  {
    fputc(character, destinationFile);
  }

  fclose(sourceFile);
  fclose(destinationFile);
}

void readGameState(FILE *file, s_game *game)
{
  fscanf(file, "%d %d %c", &game->initialDiceValueW, &game->initialDiceValueB, &game->turn);

  for (int i = 0; i < COLUMNS_COUNT; ++i)
  {
    vector_t_pawn *pawn = &game->board.columns[i].pawnIds;
    fscanf(file, " %d", &pawn->count);

    if (pawn->count > pawn->allocated_size)
    {
      reallocate(pawn, pawn->count * 2);
    }

    for (int j = 0; j < pawn->count; ++j)
    {
      fscanf(file, " %d %c", &pawn->ptr[j].id, &pawn->ptr[j].color);
    }
  }

  vector_t_pawn *barPawn = &game->board.bar.pawnIds;
  fscanf(file, " %d", &barPawn->count);

  if (barPawn->count > barPawn->allocated_size)
  {
    reallocate(barPawn, barPawn->count * 2);
  }
  for (int i = 0; i < barPawn->count; ++i)
  {
    fscanf(file, " %d %c", &barPawn->ptr[i].id, &barPawn->ptr[i].color);
  }

  fscanf(file, " %d %d %d", &game->board.isBarActive, &game->board.sourceColumn, &game->board.targetColumn);

  vector_t_pawn *courtPawns = &game->courtPawns;
  fscanf(file, " %d", &courtPawns->count);

  if (courtPawns->count > courtPawns->allocated_size)
  {
    reallocate(courtPawns, courtPawns->count * 2);
  }

  for (int i = 0; i < courtPawns->count; ++i)
  {
    fscanf(file, " %d %c", &courtPawns->ptr[i].id, &courtPawns->ptr[i].color);
  }

  // fscanf(file, " %d", &game->diceInfo.diceSize);
  // for (int i = 0; i < game->diceInfo.diceSize; ++i)
  // {
  //   fscanf(file, " %d", &game->diceInfo.dice[i]);
  // }

  // fscanf(file, " %d %d %d %d", &game->diceInfo.isDoublet, &game->diceInfo.availableDiceMoves,
  //        &game->diceInfo.initialDiceValues[0], &game->diceInfo.initialDiceValues[1]);
}

void loadFile(s_game *game)
{
  FILE *file = fopen(SAVED_GAME, "r");
  fseek(file, -2, SEEK_END); // Przesunięcie kursora na przedostatnią pozycję
  while (fgetc(file) != '\n')
  {
    fseek(file, -2, SEEK_CUR); // Przesunięcie kursora w lewo, pomijając znak nowej linii
  }

  readGameState(file, game);
  fclose(file);
}