#include "ncurses.h"
#include "stdlib.h"
#include "../headers/globalStructs.h"
#include "../headers/rollDice.h"
#include "../headers/menu.h"

#define POSITION_X 55
#define POSITION_Y 3

void showPlayerInfo(s_game game)
{
  mvprintw(POSITION_Y, POSITION_X, "Who starts?");
  mvprintw(POSITION_Y + 1, POSITION_X, "Roll -> %s", game.turn == 'w' ? "Player 1" : "Player 2");

  if (game.initialDiceValueW != -1)
  {
    mvprintw(POSITION_Y + 3, POSITION_X, "Player 1 result = %d", game.initialDiceValueW);
  }

  if (game.initialDiceValueB != -1)
  {
    mvprintw(POSITION_Y + 4, POSITION_X, "Player 2 result = %d", game.initialDiceValueB);
  }
}

void clearSidebarInfo()
{

  for (int i = 0; i < 20; ++i)
  {
    move(POSITION_Y + i, POSITION_X);
    clrtoeol();
  }
  refresh();
}

void showTurnInfo(s_game game)
{
  mvprintw(POSITION_Y, POSITION_X, "Turn -> Player %c", (game.turn == 'w') ? '1' : '2');

  mvprintw(POSITION_Y + 2, POSITION_X, "Roll dice result -> %d %d", game.diceInfo.dice[0], game.diceInfo.dice[1]);

  mvprintw(POSITION_Y + 4, POSITION_X, "Moves left:");

  int displayPosition = 0;
  for (int i = 0; i < game.diceInfo.diceSize; ++i)
  {
    int diceMove = game.diceInfo.dice[i];
    // if -1 we already used it, so just skip this step
    if (diceMove != -1)
    {
      mvprintw(POSITION_Y + 5 + displayPosition, POSITION_X, "-> %d", diceMove);
      displayPosition++;
    }
  }
}

void updateInitialDiceValues(s_game *game)
{
  showPlayerInfo(*game);

  int startRollsIds[] = {7};
  renderMenu(startRollsIds, sizeof(startRollsIds) / sizeof(startRollsIds[0]), 0, 0, 19, NULL, game);

  int player1Num = randomNum(1, 6);
  game->initialDiceValueW = player1Num;

  showPlayerInfo(*game);

  renderMenu(startRollsIds, sizeof(startRollsIds) / sizeof(startRollsIds[0]), 0, 0, 19, NULL, game);

  int player2Num = randomNum(1, 6);
  game->initialDiceValueB = player2Num;

  showPlayerInfo(*game);

  char startPlayer = player1Num >= player2Num ? 'w' : 'b';

  mvprintw(POSITION_Y + 6, POSITION_X, "Player %c starts the game", (startPlayer == 'w') ? '1' : '2');

  mvprintw(POSITION_Y + 12, POSITION_X, "Turn -> Player %c", (startPlayer == 'w') ? '1' : '2');

  game->turn = startPlayer;
}