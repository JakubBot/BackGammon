#include "ncurses.h"
#include "../headers/globalStructs.h"

void clrButtonPrints(int posY, int nLines)
{
    for (int i = 0; i < nLines; ++i)
    {
        move(posY + i, 0);
        clrtoeol();
    }
    refresh();
}

void changeTurn(s_game *game)
{
    if (game->turn == 'w')
    {
        game->turn = 'b';
    }
    else
    {
        game->turn = 'w';
    }
    game->diceInfo.isDoublet = 0;
    game->diceInfo.diceSize = 0;
    game->diceInfo.availableDiceMoves = 0;
}



s_boardColumn findColumnBasedOnColX(s_game game, int currentActiveColumn) {
    for (int i = 0; i < COLUMNS_COUNT; ++i) {
        s_boardColumn boardColumn = game.board.columns[i];

        if (boardColumn.colX == currentActiveColumn) {
            return boardColumn;
        }
    }
    return game.board.columns[0];
}

vector_t_pawn* findColumnPawnIds(s_game *game, int currentActiveColumn) {
    for (int i = 0; i < COLUMNS_COUNT; ++i) {
        s_boardColumn boardColumn = game->board.columns[i];

        if (boardColumn.colX == currentActiveColumn) {
            return &game->board.columns[i].pawnIds;
        }
    }
    return &game->board.columns[0].pawnIds;
}