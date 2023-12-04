#include "ncurses.h"
#include "stdlib.h"
#include "../headers/globalStructs.h"
#include "../headers/rollDice.h"
#include "../headers/menu.h"
#include "../headers/utils.h"

#define POSITION_X (BOARD_WIDTH + 4)
// #define POSITION_Y 9
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

void showColorsDefinition()
{
  int offsetX = 4;
  int offsetY = -4;
  int startDefinitionY = POSITION_Y + BOARD_HEIGHT + offsetY;
  int startDefinitionX = POSITION_X;

  mvprintw(startDefinitionY, startDefinitionX, "Colors definition:");
  attron(COLOR_PAIR(COLOR_COLUMN_ID));
  mvprintw(startDefinitionY + 1, startDefinitionX, SQUARE);
  attroff(COLOR_PAIR(COLOR_COLUMN_ID));
  mvprintw(startDefinitionY + 1, startDefinitionX + 1, " - current column");

  attron(COLOR_PAIR(POSSIBLE_COLUMN_MOVE_ID));
  mvprintw(startDefinitionY + 2, startDefinitionX, SQUARE);
  attroff(COLOR_PAIR(POSSIBLE_COLUMN_MOVE_ID));
  mvprintw(startDefinitionY + 2, startDefinitionX + 1, " - source column");

  attron(COLOR_PAIR(FORCED_COLUMN_MOVE_ID));
  mvprintw(startDefinitionY + 3, startDefinitionX, SQUARE);
  attroff(COLOR_PAIR(FORCED_COLUMN_MOVE_ID));
  mvprintw(startDefinitionY + 3, startDefinitionX + 1, " - forced target column");

}

void showTurnInfo(s_game game)
{
  mvprintw(POSITION_Y, POSITION_X, "Turn -> Player %c", (game.turn == 'w') ? '1' : '2');

  mvprintw(POSITION_Y + 2, POSITION_X, "Roll dice result -> %d %d", game.diceInfo.initialDiceValues[0], game.diceInfo.initialDiceValues[1]);

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
  showColorsDefinition();
}

void updateInitialDiceValues(s_game *game)
{
  showPlayerInfo(*game);

  int rollId[] = {7};
  int rollSize = sizeof(rollId) / sizeof(rollId[0]);
  renderMenu(rollId, rollSize, 0, 0, 19, NULL, game);

  int p1 = randomNum(1, 6);
  game->initialDiceValueW = p1;

  showPlayerInfo(*game);

  renderMenu(rollId, rollSize, 0, 0, 19, NULL, game);

  int p2 = randomNum(1, 6);
  game->initialDiceValueB = p2;

  showPlayerInfo(*game);

  char startCol = p1 >= p2 ? 'w' : 'b';

  mvprintw(POSITION_Y + 6, POSITION_X, "Player %c starts the game", (startCol == 'w') ? '1' : '2');

  mvprintw(POSITION_Y + 12, POSITION_X, "Turn -> Player %c", (startCol == 'w') ? '1' : '2');

  game->turn = startCol;
}