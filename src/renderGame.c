
#include "ncurses.h"
#include "stdlib.h"
#include "../headers/menu.h"
#include "../headers/vectorPawn.h"
#include "../headers/playerTurn.h"
#include "../headers/globalStructs.h"
#include "../headers/utils.h"
#include "../headers/move.h"

int getPawnInitialX(char color, int index)
{
    int firstColBreakPoint = 2;
    int secondColBreakPoint = firstColBreakPoint + 5;
    int thirdColBreakPoint = secondColBreakPoint + 3;
    int fourthColBreakPoint = thirdColBreakPoint + 2;

    int revertPosition = color == 'w' ? 0 : COLUMNS_COUNT - 1;
    if (color == 'w')
    {
        if (index <= firstColBreakPoint)
        {
            return 0;
        }
        else if (secondColBreakPoint)
        {
            return abs(revertPosition - 11);
        }
        else if (thirdColBreakPoint)
        {
            return abs(revertPosition - 16);
        }
        else if (fourthColBreakPoint)
        {
            return abs(revertPosition - 18);
        }
    }
    return color == 'w' ? 0 : COLUMNS_COUNT - 1;
}

void getInitialColumnsIds(vector_t_pawn *boardColumn, int column)
{
    char color = column == 0 || column == 11 || column == 16 || column == 18 ? 'b' : 'w';
    if (column == 11 || 23 == column)
    {
        push_back(boardColumn, (s_pawn){.id = 0, .color = color});
        push_back(boardColumn, (s_pawn){.id = 1, .color = color});
    }
    else if (column == 0 || column == 12)
    {
        push_back(boardColumn, (s_pawn){.id = 2, .color = color});
        push_back(boardColumn, (s_pawn){.id = 3, .color = color});
        push_back(boardColumn, (s_pawn){.id = 4, .color = color});
        push_back(boardColumn, (s_pawn){.id = 5, .color = color});
        push_back(boardColumn, (s_pawn){.id = 6, .color = color});
    }
    else if (column == 4 || column == 16)
    {
        push_back(boardColumn, (s_pawn){.id = 12, .color = color});
        push_back(boardColumn, (s_pawn){.id = 13, .color = color});
        push_back(boardColumn, (s_pawn){.id = 14, .color = color});
    }
    else if (column == 6 || column == 18)
    {
        push_back(boardColumn, (s_pawn){.id = 7, .color = color});
        push_back(boardColumn, (s_pawn){.id = 8, .color = color});
        push_back(boardColumn, (s_pawn){.id = 9, .color = color});
        push_back(boardColumn, (s_pawn){.id = 10, .color = color});
        push_back(boardColumn, (s_pawn){.id = 11, .color = color});
    }
}

void initializeGame(s_game *game)
{
    game->turn = 'w';
    game->initialDiceValueW = -1;
    game->initialDiceValueB = -1;

    game->board.sourceColumn = -1;
    game->board.targetColumn = -1;

    vector_t_pawn *barPawn = &game->board.bar.pawnIds;

    game->board.isBarActive = 0;

    init(barPawn);

    for (int i = 0; i < COLUMNS_COUNT; ++i)
    {
        int isReversed = (i + 1) > (COLUMNS_COUNT / 2) ? 1 : 0;
        int colX = isReversed ? COLUMNS_COUNT - i - 1 : (COLUMNS_COUNT / 2) + i;

        game->board.columns[i].colX = colX;
        game->board.columns[i].colY = isReversed ? 1 : 0;
        game->board.columns[i].isReversed = isReversed;
        vector_t_pawn *pawn = &game->board.columns[i].pawnIds;

        init(pawn);

        getInitialColumnsIds(pawn, i);

        refresh();
    }

    for (int i = 0; i < PAWNS_COUNT; ++i)
    {
        game->wPawns[i].color = 'w';
        game->wPawns[i].id = i;

        game->bPawns[i].color = 'b';
        game->bPawns[i].id = i;
    }
}

void renderColumn(s_boardColumn boardColumn, s_game game, WINDOW *gameWin, int additionalOffsetLeft)
{
    int x = boardColumn.colX, y = boardColumn.colY, reversed = boardColumn.isReversed;
    // reversed 1 means that we want to reverse column
    // to avoid border
    int offset_y = 4;
    int offset_x = 1 + additionalOffsetLeft;
    int columnVerticalOffset = 4;
    int labelRowId = x + 1;
    char *label = (x % 2) == 1 ? reversed ? "/\\" : "\\/" : "--";

    vector_t_pawn currentColumn = boardColumn.pawnIds;
    int columnCount = currentColumn.count;
    char color = currentColumn.ptr[0].color;
    char *pawnLabel = color == 'b' ? BLACK_PAWN : WHITE_PAWN;
    // char *pawnLabel = color == 'b' ?  "ww" : "bb";

    int count = BOARD_COLUMN_HEIGHT - columnCount;
    if (reversed)
    {
        x = (COLUMNS_COUNT / 2) - x - 1;

        for (int row = 0; row < BOARD_COLUMN_HEIGHT; ++row)
        {
            // normal chars
            int yOffset = row + offset_y + columnVerticalOffset + COLUMNGAP;
            int xOffset = offset_x + (x * 4);
            if (count > 0)
            {
                mvprintw(yOffset, xOffset, label);
                count--;
            }
            else
            {
                mvprintw(yOffset, xOffset, pawnLabel);
            }
        }
        mvprintw(BOARD_COLUMN_HEIGHT + offset_y + columnVerticalOffset + COLUMNGAP + 1, offset_x + (x * 4), "%d", labelRowId);
    }
    else
    {
        x = x - (COLUMNS_COUNT / 2);

        for (int row = 0; row < BOARD_COLUMN_HEIGHT; ++row)
        {
            // normal chars
            int yOffset = row + offset_y;
            int xOffset = offset_x + (x * 4);
            if (columnCount > 0)
            {
                mvprintw(yOffset, xOffset, pawnLabel);
                columnCount--;
            }
            else
            {
                mvprintw(yOffset, xOffset, label);
            }
        }
        mvprintw(offset_y - 2, offset_x + (x * 4), "%d", labelRowId);
    }
}

int findNextPossibleMove(s_game game, int currentActiveColumn, char action)
{
    int currIndex = currentActiveColumn;
    while (true)
    {
        if (currIndex >= COLUMNS_COUNT)
        {
            currIndex = 0;
        }
        else if (currIndex < 0)
        {
            currIndex = COLUMNS_COUNT - 1;
        }

        s_boardColumn bColumn = findColumnBasedOnColX(game, currIndex);
        vector_t_pawn cColumn = bColumn.pawnIds;

        if ((cColumn.count > 0) && (bColumn.colX != game.board.sourceColumn))
        {
            char color = cColumn.ptr[0].color;
            if ((game.turn == 'w' && color == 'w') || game.turn == 'b' && color == 'b')
            {
                return bColumn.colX;
            }
        }
        if (action == 'r')
        {
            ++currIndex;
        }
        else if (action == 'l')
        {
            --currIndex;
        }
    }
}

void findColumnWithPawn(s_game game, int *currentActiveColumn, char action)
{
    switch (action)
    {
    case 'r':
        *currentActiveColumn = findNextPossibleMove(game, *currentActiveColumn + 1, action);
        break;
    case 'l':
        *currentActiveColumn = findNextPossibleMove(game, *currentActiveColumn - 1, action);
        break;

    default:
        // just find any right column starting with STARTING_COLUMN_POINT(12) index (based on colX)
        *currentActiveColumn = findNextPossibleMove(game, STARTING_COLUMN_POINT, 'r');
        break;
    }
}
int getNextMoveCalculation(int firstVal, int secondVal, int whiteTurn, char action, int changeDirection)
{
    //        if ((action == 'r' && whiteTurn) || (action == 'l' && !whiteTurn)) {
    if (whiteTurn)
    {
        if (action == 'l' || changeDirection)
        {
            return firstVal + secondVal;
        }
        else
        {
            return firstVal - secondVal;
        }
    }
    else
    {
        if (action == 'l' || changeDirection)
        {
            return firstVal - secondVal;
        }
        else
        {
            return firstVal + secondVal;
        }
    }
}

int validPawnToColumnMove(s_game *game, int nextColX)
{
    s_boardColumn nextCol = findColumnBasedOnColX(*game, nextColX);

    if (nextCol.pawnIds.count >= 2)
    {
        if (nextCol.pawnIds.ptr[0].color != game->turn)
        {
            return 0;
        }
    }
    else if (nextCol.pawnIds.count == 1)
    {
        //        capturePawn(game,nextColX);
        return 1;
    }

    return 1;
}

int findNextLegalMove(s_game *game, int *currentActiveColumn, char action, int *targetColStep, int notFoundCount)
{
    int sourceColX = game->board.sourceColumn;

    int *dice = game->diceInfo.dice;

    short whiteTurn = game->turn == 'w' ? 1 : 0;

    int stepValue = dice[0];

    if (game->diceInfo.isDoublet)
    {
        if (action == 'r' && ((*targetColStep) > 1))
        {
            (*targetColStep)--;
            *currentActiveColumn = getNextMoveCalculation(*currentActiveColumn, stepValue, whiteTurn, action, 0);
        }
        else if (action == 'l' && (*targetColStep < game->diceInfo.availableDiceMoves))
        {
            int nextMove = getNextMoveCalculation(*currentActiveColumn, stepValue, whiteTurn, action, 0);
            if ((nextMove < 0) || (nextMove >= COLUMNS_COUNT))
            {
                return 0;
            }
            (*targetColStep)++;
            *currentActiveColumn = nextMove;
        }
        return 0;
    }

    int indexMove = 0;
    int dice1 = dice[0];
    int dice2 = dice[1];
    int maxDice, minDice;

    int increaseStep = 0;

    // only 1 value left
    if (game->diceInfo.availableDiceMoves == 1)
    {
        int val = dice[0] == -1 ? dice[1] : dice[0];

        if (action == 'r')
        {
            if (*targetColStep > 0)
            {
                increaseStep = -1;
            }
        }
        else
        {
            indexMove = val;
            increaseStep = 1;
        }
    }
    else
    {
        if (dice1 >= dice2)
        {
            maxDice = dice1;
            minDice = dice2;
        }
        else
        {
            maxDice = dice2;
            minDice = dice1;
        }
        // initial position
        if (*targetColStep == 0)
        {
            if (action == 'l')
            {
                indexMove = minDice;
                increaseStep = 1;
            }
        }
        else if (*targetColStep == 1)
        {
            if (action == 'r')
            {
                increaseStep = -1;
            }
            else
            {
                indexMove = maxDice;
                increaseStep = 1;
            }
        }
        else if (*targetColStep == 2)
        {
            if (action == 'r')
            {
                indexMove = minDice;
                increaseStep = -1;
            }
            else
            {
                indexMove = maxDice + minDice;
                increaseStep = 1;
            }
        }
        else if (*targetColStep == 3)
        {
            if (action == 'r')
            {
                indexMove = maxDice;
                increaseStep = -1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            indexMove = minDice;
        }
    }

    int move = getNextMoveCalculation(sourceColX, indexMove, whiteTurn, action, 1);

    int pawnGoToHome = allPawnsInHome(*game) && ((move == -1) || (move == COLUMNS_COUNT));

    if ((move < 0 || move > (COLUMNS_COUNT - 1)) && !pawnGoToHome) {
        return 0;
    }

    int isValidColumn = validPawnToColumnMove(game, move);

    if (increaseStep == 1)
    {
        (*targetColStep)++;
    }
    else if (increaseStep == -1)
    {
        (*targetColStep)--;
    }

    if (!isValidColumn)
    {
        if (*targetColStep >= 4 || notFoundCount == 1)
        {
            *targetColStep = 0;
            refresh();
            return 0;
        }

        return findNextLegalMove(game, currentActiveColumn, action, targetColStep ,notFoundCount + 1);
    }

    *currentActiveColumn = move;

    return 0;
}

void addAdditionalEntrance(s_game game) {
    // additional square for start and end for each color
    if (game.board.sourceColumn == 24 && game.board.isBarActive) {
        attron(COLOR_PAIR(2));
        mvprintw(4,53, SQUARE);
        attroff(COLOR_PAIR(2));
    } else {
        mvprintw(4,53, " ");
    }

    if (game.board.sourceColumn == -1 && game.board.isBarActive) {
        attron(COLOR_PAIR(2));
        mvprintw(15,53, SQUARE);
        attroff(COLOR_PAIR(2));
    }else {
        mvprintw(15,53, " ");
    }

    refresh();
}

void renderBoard(s_game *game, WINDOW *gameWin, int selectColumn, int targetColumn)
{
    // selectColumn - 1 to active
    // currentActiveColumn represents column(colX) that is active
    int currentActiveColumn = -1;
    int targetColStep = 0;
    do
    {
        if (targetColumn && currentActiveColumn == -1)
        {
            currentActiveColumn = game->board.sourceColumn;
        }
        else if (selectColumn && currentActiveColumn == -1)
        {
            findColumnWithPawn(*game, &currentActiveColumn, ' ');
        }

        addAdditionalEntrance(*game);

        for (int i = 0; i < COLUMNS_COUNT; ++i)
        {
            s_boardColumn boardColumn = game->board.columns[i];
            int additionalOffsetLeft = 0;
            if (boardColumn.colX < 6 || boardColumn.colX > 17)
            {
                additionalOffsetLeft = 4;
            }
            if ((boardColumn.colX == currentActiveColumn) || (boardColumn.colX == game->board.sourceColumn))
            {
                int colorPair = boardColumn.colX == currentActiveColumn ? 2 : 3;
                attron(COLOR_PAIR(colorPair));
                renderColumn(boardColumn, *game, gameWin, additionalOffsetLeft);

                attroff(COLOR_PAIR(colorPair));
            }
            else
            {
                renderColumn(boardColumn, *game, gameWin, additionalOffsetLeft);
            }
        }
        // render bar that is between 6 and 7 column
        int barElements = (MAX_PAWN_ON_COL * 2) + COLUMNGAP;
        int offset_y = 3;
        //        WHITE_PAWN
        int barPawnCount = game->board.bar.pawnIds.count;
        for (int i = 1; i < barElements; ++i)
        {
            mvprintw(i + offset_y, 1 + (6 * 4), "||");
        }
        vector_t_pawn *pawn = &game->board.bar.pawnIds;

        int start = (barElements / 2) - (barPawnCount / 2);

        for (int i = 0; i < barPawnCount; ++i)
        {

            char clr = pawn->ptr[0].color;

            char *pawnLabel = clr == 'b' ? BLACK_PAWN : WHITE_PAWN;

            if (i == 0 && game->board.isBarActive)
            {
                attron(COLOR_PAIR(2));
                mvprintw(i + offset_y + start, 1 + (6 * 4), pawnLabel);
                attroff(COLOR_PAIR(2));
            }
            else
            {
                mvprintw(i + offset_y + start, 1 + (6 * 4), pawnLabel);
            }
        }


        wrefresh(gameWin);

        if (selectColumn == 1)
        {
            int ch = getch();
            curs_set(0);
            if (ch == KEY_LEFT)
            {
                findColumnWithPawn(*game, &currentActiveColumn, 'l');
            }
            else if (ch == KEY_RIGHT)
            {
                findColumnWithPawn(*game, &currentActiveColumn, 'r');
            }
            else if (ch == 10)
            {
                break;
            }
        }
        else if (targetColumn == 1)
        {
            int ch = getch();
            curs_set(0);
            if (ch == KEY_LEFT)
            {
                findNextLegalMove(game, &currentActiveColumn, 'l', &targetColStep,0);
            }
            else if (ch == KEY_RIGHT)
            {
                findNextLegalMove(game, &currentActiveColumn, 'r', &targetColStep,0);
            }
            else if (ch == 10)
            {
                break;
            }
        }

    } while (selectColumn || targetColumn);

    if (selectColumn)
    {
        game->board.sourceColumn = currentActiveColumn;
    }
    else if (targetColumn)
    {
        game->board.targetColumn = currentActiveColumn;
        if (targetColStep == 0)
        {
        }
        else if (game->diceInfo.isDoublet)
        {
            game->diceInfo.availableDiceMoves = game->diceInfo.availableDiceMoves - targetColStep;
            for (int i = 4; i > game->diceInfo.availableDiceMoves; --i)
            {
                game->diceInfo.dice[i - 1] = -1;
            }
        }
        else
        {
            if (targetColStep == 3)
            {
                game->diceInfo.availableDiceMoves = 0;
            }
            else
            {
                game->diceInfo.availableDiceMoves = game->diceInfo.availableDiceMoves - 1;
                int dice1 = game->diceInfo.dice[0];
                int dice2 = game->diceInfo.dice[1];
                int lowIndex = 0;
                int highIndex = 0;
                if (dice1 >= dice2)
                {
                    highIndex = 0;
                    lowIndex = 1;
                }
                else
                {
                    highIndex = 1;
                    lowIndex = 0;
                }
                if (targetColStep == 1)
                {
                    game->diceInfo.dice[lowIndex] = -1;
                }
                else
                {
                    game->diceInfo.dice[highIndex] = -1;
                }
            }
        }
        clearSidebarInfo();
        showTurnInfo(*game);
    }
}

int shouldUseBarPawn(s_game game) {
    if (game.board.bar.pawnIds.count > 0 && game.board.bar.pawnIds.ptr[0].color == game.turn) {
        return 1;
    }
    return 0;
}

void moveRepeater(s_game *game, WINDOW *gameWin)
{
    while (game->diceInfo.availableDiceMoves > 0)
    {
        if (shouldUseBarPawn(*game))
        {
            game->board.isBarActive = 1;
            if (game->turn == 'w') {
                game->board.sourceColumn = -1;
            } else {
                game->board.sourceColumn = 24;
            }
        }
        else
        {
            game->board.isBarActive = 0;
            int sourceMoveIds[] = {5, 8};
            clrButtonPrints(19, 3);
            refresh();
            renderMenu(sourceMoveIds, sizeof(sourceMoveIds) / sizeof(sourceMoveIds[0]), 0, 0, 19, NULL, game);
            renderBoard(game, gameWin, 1, 0);
            clrButtonPrints(19, 3);
        }

        // render a target for a pawn
        int targetMoveIds[] = {6, 8};
        renderMenu(targetMoveIds, sizeof(targetMoveIds) / sizeof(targetMoveIds[0]), 0, 0, 19, NULL, game);
        renderBoard(game, gameWin, 0, 1);

        movePawn(game);

        renderBoard(game, gameWin, 0, 0);
    }
}

void gameLoop(s_game game, WINDOW *gameWin)
{
    // this will render menu with roll dice option
    int menuIds[] = {4, 8};
    int diceSize = 0;
    clrButtonPrints(19, 3);
    renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), 0, 0, 19, &diceSize, &game);

    curs_set(0);

    moveRepeater(&game, gameWin);

    changeTurn(&game);

    refresh();
    wrefresh(gameWin);
    clearSidebarInfo();

    gameLoop(game, gameWin);
}

void renderGame()
{
    erase();
    refresh();

    mvprintw(0, 0, "Backgammon (press q to exit)");
    s_game game = {};
    initializeGame(&game);

    int ySize = 14, xSize = 52, yStart = 3, xStart = 0;
    WINDOW *gameWin = newwin(ySize, xSize, yStart, xStart);
    wrefresh(gameWin);
    box(gameWin, 0, 0);
    wrefresh(gameWin);
    renderBoard(&game, gameWin, 0, 0);

    refresh();
    updateInitialDiceValues(&game);

    wrefresh(gameWin);

    gameLoop(game, gameWin);
}
