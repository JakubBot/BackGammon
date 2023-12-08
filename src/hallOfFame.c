#include "ncurses.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "../headers/globalStructs.h"

#define USER_WINDOW_X 0
#define USER_WINDOW_Y 2

void selectsUserAccount(s_game *game);

s_player *findUser(s_game game, char userName[])
{
  s_player *users = game.usersData.allPlayers;
  int userCount = game.usersData.userCount;
  for (int i = 0; i < userCount; ++i)
  {
    if (strcmp(users[i].name, userName) == 0)
    {
      return &users[i];
    }
  }
  return NULL;
}

void headerToSelectUsers(s_game game)
{
  mvprintw(USER_WINDOW_Y, 0, "Select(type) your account, or to create new one write 'new'");
  if (game.usersData.firstPlayerSelects)
  {
    mvprintw(USER_WINDOW_Y + 2, 0, "User 1");
  }
  else
  {
    mvprintw(USER_WINDOW_Y + 2, 0, "User 2");
  }
}

void printPossibleUsers(s_game game)
{
  refresh();
  // mvprintw(USER_WINDOW_Y, 0, "Select(type) your account, or to create new one write 'new'");
  // if (game.usersData.firstPlayerSelects)
  // {
  //   mvprintw(USER_WINDOW_Y + 2, 0, "User 1");
  // }
  // else
  // {
  //   mvprintw(USER_WINDOW_Y + 2, 0, "User 2");
  // }
  headerToSelectUsers(game);
  s_player *users = game.usersData.allPlayers;
  int userCount = game.usersData.userCount;

  int secondplayer = game.usersData.firstPlayerSelects == 0 ? 1 : 0;
  refresh();

  int idx = 0;
  for (int i = 0; i < userCount; ++i)
  {
    if (strcmp(users[i].name, game.usersData.player1.name) == 0)
    {
      continue;
    }

    mvprintw(idx + USER_WINDOW_Y + 4, 0, "%s %d\n", users[i].name, users[i].score);
    idx++;
  }
}

void clearTerminal()
{
  move(1, 0);
  clrtobot();
  refresh();
}
void addNewUser(s_game *game)
{
  clearTerminal();

  mvprintw(USER_WINDOW_Y, 0, "Podaj nazwe nowego uzytkownika:");

  FILE *file = fopen(USERS, "a");
  if (file == NULL)
  {
    return;
  }

  char inputBuffer[100];

  move(USER_WINDOW_Y + 2, 0);
  getstr(inputBuffer);

  fprintf(file, "%s %d\n", inputBuffer, 0);
  fclose(file);
}

void assignUserData(s_game *game, s_player *user)
{
  if (game->usersData.firstPlayerSelects == 1)
  {
    strcpy(game->usersData.player1.name, user->name);
    game->usersData.player1.score = user->score;
  }
  else
  {
    strcpy(game->usersData.player2.name, user->name);

    game->usersData.player2.score = user->score;
  }
}

void getUserName(s_game *game)
{
  echo();

  int userCount = game->usersData.userCount;

  char inputBuffer[100];

  move(USER_WINDOW_Y + userCount + 5, 0);

  clrtoeol();
  getstr(inputBuffer);

  if (strcmp(inputBuffer, "new") == 0)
  {
    addNewUser(game);
    selectsUserAccount(game);
    return;
  }

  s_player *selectedUser = findUser(*game, inputBuffer);

  if (selectedUser == NULL)
  {
    mvprintw(USER_WINDOW_Y + userCount + 5 + 2, 0, "Invalid user name");
    return getUserName(game);
  }
  else
  {
    assignUserData(game, selectedUser);
  }

  noecho();
}

void fetchUsers(s_game *game)
{
  FILE *file = fopen(USERS, "r");
  if (file == NULL)
  {
    return;
  }

  s_player users[MAX_USERS_COUNT];

  int userCount = 0;
  while (fscanf(file, "%s %d", users[userCount].name, &users[userCount].score) == 2)
  {
    userCount++;

    if (userCount >= MAX_USERS_COUNT)
    {
      fprintf(stderr, "Przekroczono maksymalną liczbę użytkowników\n");
      break;
    }
  }
  fclose(file);

  for (int i = 0; i < userCount; ++i)
  {
    game->usersData.allPlayers[i] = users[i];
  }
  game->usersData.userCount = userCount;
}

void selectsUserAccount(s_game *game)
{
  // FILE *file = fopen(USERS, "r");
  // if (file == NULL)
  // {
  //   return;
  // }

  // s_player users[MAX_USERS_COUNT];

  // int userCount = 0;
  // while (fscanf(file, "%s %d", users[userCount].name, &users[userCount].score) == 2)
  // {
  //   userCount++;

  //   if (userCount >= MAX_USERS_COUNT)
  //   {
  //     fprintf(stderr, "Przekroczono maksymalną liczbę użytkowników\n");
  //     break;
  //   }
  // }
  // fclose(file);

  // for (int i = 0; i < userCount; ++i)
  // {
  //   game->usersData.allPlayers[i] = users[i];
  // }
  // game->usersData.userCount = userCount;
  fetchUsers(game);

  printPossibleUsers(*game);
  getUserName(game);

  int firstPlayerSelects = game->usersData.firstPlayerSelects;
  if (firstPlayerSelects)
  {
    game->usersData.firstPlayerSelects = 0;
    clearTerminal();
    selectsUserAccount(game);
  }
  clearTerminal();
}

void updateCurrentSavedUser(s_game *game)
{
  if (game->endGame.winner == 1)
  {
    game->usersData.player1.score = game->usersData.player1.score + 1;
  }
  else
  {
    game->usersData.player2.score = game->usersData.player2.score + 1;
  }
}

void updateWinnerScore(s_game *game)
{
  int additionalPoints = 1;
  s_player *winner = game->endGame.winner == 1 ? &game->usersData.player1 : &game->usersData.player2;

  FILE *file = fopen(USERS, "w");
  if (file == NULL)
  {
    return;
  }

  s_player *users = game->usersData.allPlayers;
  int userCount = game->usersData.userCount;

  for (int i = 0; i < userCount; ++i)
  {
    if (strcmp(users[i].name, winner->name) == 0)
    {
      fprintf(file, "%s %d\n", users[i].name, users[i].score + additionalPoints);
      continue;
    }
    fprintf(file, "%s %d\n", users[i].name, users[i].score);
  }
  fclose(file);

  fetchUsers(game);

  updateCurrentSavedUser(game);
}