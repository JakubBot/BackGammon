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
    fprintf(file, " %d", barPawn->ptr[i].id);
  }

  fprintf(file, " %d %d %d", game.board.isBarActive, game.board.sourceColumn, game.board.targetColumn);
  fprintf(file, " %d", game.diceInfo.diceSize);
  for (int i = 0; i < game.diceInfo.diceSize; ++i)
  {
    fprintf(file, " %d", game.diceInfo.dice[i]);
  }

  fprintf(file, " %d %d %d %d", game.diceInfo.isDoublet, game.diceInfo.availableDiceMoves,
          game.diceInfo.initialDiceValues[0], game.diceInfo.initialDiceValues[1]);

  fprintf(file, "\n");
}

void updateGameFile(int copyToCurrentFile)
{
  char *source = copyToCurrentFile ? SAVED_GAME : CURRENT_GAME;
  char *target = copyToCurrentFile ? CURRENT_GAME : SAVED_GAME;
  FILE *sourceFile = fopen(source, "r");    
  FILE *destinationFile = fopen(target, "w"); 

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
    for (int j = 0; j < pawn->count; ++j)
    {
      fscanf(file, " %d %c", &pawn->ptr[j].id, &pawn->ptr[j].color);
    }
  }

  vector_t_pawn *barPawn = &game->board.bar.pawnIds;
  fscanf(file, " %d", &barPawn->count);
  for (int i = 0; i < barPawn->count; ++i)
  {
    fscanf(file, " %d", &barPawn->ptr[i].id);
  }

  fscanf(file, " %d %d %d", &game->board.isBarActive, &game->board.sourceColumn, &game->board.targetColumn);
  fscanf(file, " %d", &game->diceInfo.diceSize);
  for (int i = 0; i < game->diceInfo.diceSize; ++i)
  {
    fscanf(file, " %d", &game->diceInfo.dice[i]);
  }

  fscanf(file, " %d %d %d %d", &game->diceInfo.isDoublet, &game->diceInfo.availableDiceMoves,
         &game->diceInfo.initialDiceValues[0], &game->diceInfo.initialDiceValues[1]);
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