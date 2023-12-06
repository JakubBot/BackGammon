#include "stdlib.h"
#include "../headers/globalStructs.h"
#include "../headers/utils.h"

void savePawnsToFile(FILE *file, vector_t_pawn *pawns)
{
  fprintf(file, " %d", pawns->count);
  for (int j = 0; j < pawns->count; ++j)
  {
    fprintf(file, " %d %c", pawns->ptr[j].id, pawns->ptr[j].color);
  }
}

void saveCurrentState(s_game game)
{
  FILE *file = game.file;

  fprintf(file, "%d %d %c", game.initialDiceValueW, game.initialDiceValueB, game.turn);

  for (int i = 0; i < COLUMNS_COUNT; ++i)
  {
    vector_t_pawn *pawn = &game.board.columns[i].pawnIds;
    savePawnsToFile(file, pawn);
    // fprintf(file, " %d", pawn->count);
    // for (int j = 0; j < pawn->count; ++j)
    // {
    //   fprintf(file, " %d %c", pawn->ptr[j].id, pawn->ptr[j].color);
    // }
  }

  vector_t_pawn *barPawn = &game.board.bar.pawnIds;
  savePawnsToFile(file, barPawn);
  // fprintf(file, " %d", barPawn->count);
  // for (int i = 0; i < barPawn->count; ++i)
  // {
  //   fprintf(file, " %d %c", barPawn->ptr[i].id, barPawn->ptr[i].color);
  // }

  vector_t_pawn *courtPawns = &game.courtPawns;
  savePawnsToFile(file, courtPawns);

  fprintf(file, " %d %d %d", game.board.isBarActive, game.board.sourceColumn, game.board.targetColumn);

  // fprintf(file, " %d", courtPawns->count);
  // for (int i = 0; i < courtPawns->count; ++i)
  // {
  //   fprintf(file, " %d %c", courtPawns->ptr[i].id, courtPawns->ptr[i].color);
  // }

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

  sourceFile = fopen(source, "r");
  destinationFile = fopen(target, "w");

  if (sourceFile == NULL || destinationFile == NULL)
    return;

  int character;

  while ((character = fgetc(sourceFile)) != EOF)
  {
    fputc(character, destinationFile);
  }

  fclose(sourceFile);
  fclose(destinationFile);
}

void readPawnsFromFile(FILE *file, vector_t_pawn *pawns)
{
  fscanf(file, " %d", &pawns->count);

  if (pawns->count > pawns->allocated_size)
  {
    reallocate(pawns, pawns->count * 2);
  }

  for (int j = 0; j < pawns->count; ++j)
  {
    fscanf(file, " %d %c", &pawns->ptr[j].id, &pawns->ptr[j].color);
  }
}

void readGameState(FILE *file, s_game *game)
{
  fscanf(file, "%d %d %c", &game->initialDiceValueW, &game->initialDiceValueB, &game->turn);

  for (int i = 0; i < COLUMNS_COUNT; ++i)
  {
    vector_t_pawn *pawn = &game->board.columns[i].pawnIds;
    readPawnsFromFile(file, pawn);
    // fscanf(file, " %d", &pawn->count);

    // if (pawn->count > pawn->allocated_size)
    // {
    //   reallocate(pawn, pawn->count * 2);
    // }

    // for (int j = 0; j < pawn->count; ++j)
    // {
    //   fscanf(file, " %d %c", &pawn->ptr[j].id, &pawn->ptr[j].color);
    // }
  }

  vector_t_pawn *barPawn = &game->board.bar.pawnIds;
  readPawnsFromFile(file, barPawn);
  // fscanf(file, " %d", &barPawn->count);

  // if (barPawn->count > barPawn->allocated_size)
  // {
  //   reallocate(barPawn, barPawn->count * 2);
  // }
  // for (int i = 0; i < barPawn->count; ++i)
  // {
  //   fscanf(file, " %d %c", &barPawn->ptr[i].id, &barPawn->ptr[i].color);
  // }

  vector_t_pawn *courtPawns = &game->courtPawns;
  readPawnsFromFile(file, courtPawns);

  fscanf(file, " %d %d %d", &game->board.isBarActive, &game->board.sourceColumn, &game->board.targetColumn);

  // fscanf(file, " %d", &courtPawns->count);

  // if (courtPawns->count > courtPawns->allocated_size)
  // {
  //   reallocate(courtPawns, courtPawns->count * 2);
  // }

  // for (int i = 0; i < courtPawns->count; ++i)
  // {
  //   fscanf(file, " %d %c", &courtPawns->ptr[i].id, &courtPawns->ptr[i].color);
  // }
}

void loadFile(s_game *game, int loadPosition)
{
  FILE *file = fopen(SAVED_GAME, "r");

  if (file == NULL)
  {

    mvprintw(ERROR_PRINT_Y, ERROR_PRINT_X, "Nie można otworzyć pliku ");
    return;
  }

  if (loadPosition >= 0)
  {
    fseek(file, 0, SEEK_SET); // Przesunięcie kursora na początek pliku

    int currentLine = 0;
    char currentChar;

    while (currentLine < loadPosition)
    {
      currentChar = fgetc(file);

      if (currentChar == EOF)
      {
        // Obsługa błędu - osiągnięcie końca pliku przed osiągnięciem żądanego wiersza
        fclose(file);
        return;
      }

      if (currentChar == '\n')
      {
        currentLine++;
      }
    }
  }
  else
  {
    fseek(file, -2, SEEK_END); // Przesunięcie kursora na przedostatnią pozycję
    while (fgetc(file) != '\n')
    {
      fseek(file, -2, SEEK_CUR); // Przesunięcie kursora w lewo, pomijając znak nowej linii
    }
  }

  readGameState(file, game);
  fclose(file);
}

int countLines(FILE *file)
{
  int count = 0;
  int ch;

  while ((ch = fgetc(file)) != EOF)
  {
    if (ch == '\n')
    {
      count++;
    }
  }

  // Dodaj jedną do count, jeśli plik nie zakończył się znakiem nowej linii
  if (ch != '\n' && count > 0)
  {
    count++;
  }

  return count;
}