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

void savePlayersData(FILE *file, s_game game)
{
  fprintf(file, " %s %d", game.usersData.player1.name, game.usersData.player1.score);
  fprintf(file, " %s %d", game.usersData.player2.name, game.usersData.player2.score);
}

void readPlayersData(FILE *file, s_game *game)
{
  fscanf(file, " %s %d", game->usersData.player1.name, &game->usersData.player1.score);
  fscanf(file, " %s %d", game->usersData.player2.name, &game->usersData.player2.score);
}

void saveCurrentState(s_game game)
{
  FILE *file = game.file;

  fprintf(file, "%d %d %c", game.initialDiceValueW, game.initialDiceValueB, game.turn);

  for (int i = 0; i < COLUMNS_COUNT; ++i)
  {
    vector_t_pawn *pawn = &game.board.columns[i].pawnIds;
    savePawnsToFile(file, pawn);
  }

  vector_t_pawn *barPawn = &game.board.bar.pawnIds;
  savePawnsToFile(file, barPawn);

  vector_t_pawn *courtPawns = &game.courtPawns;
  savePawnsToFile(file, courtPawns);

  fprintf(file, " %d %d %d", game.board.isBarActive, game.board.sourceColumn, game.board.targetColumn);

  savePlayersData(file, game);
  fprintf(file, "\n");
}

void updateGameFile(int copyToCurrentFile, s_game game)
{
  char *source = copyToCurrentFile ? SAVED_GAME : CURRENT_GAME;
  char *target = copyToCurrentFile ? CURRENT_GAME : SAVED_GAME;
  FILE *sourceFile = NULL;
  FILE *destinationFile = NULL;

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
  }

  vector_t_pawn *barPawn = &game->board.bar.pawnIds;
  readPawnsFromFile(file, barPawn);

  vector_t_pawn *courtPawns = &game->courtPawns;
  readPawnsFromFile(file, courtPawns);

  fscanf(file, " %d %d %d", &game->board.isBarActive, &game->board.sourceColumn, &game->board.targetColumn);

  readPlayersData(file, game);
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

  if (ch != '\n' && count > 0)
  {
    count++;
  }

  return count;
}