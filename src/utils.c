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

char checkTurn(s_game game) {
    return game.turn;
}

int allPawnsInHome(s_game game) {
    if (game.board.isBarActive) {
        return 0;
    }

    char turn = checkTurn(game);

    int eWhiteHome = 6;
    int sBlackHome = 18;
    if (turn == 'w') {
        for (int i = eWhiteHome; i < PAWNS_COUNT; ++i) {
            s_boardColumn currentCol = findColumnBasedOnColX(game, i);
            vector_t_pawn pawn = currentCol.pawnIds;
            if (pawn.count > 0 && pawn.ptr[0].color != turn) {
                return 0;
            }
        }
    } else {
        for (int i = 0; i < sBlackHome; ++i) {
            s_boardColumn currentCol = findColumnBasedOnColX(game, i);
            vector_t_pawn pawn = currentCol.pawnIds;
            if (pawn.count > 0 && pawn.ptr[0].color != turn) {
                return 0;
            }
        }
    }

    return 1;
}