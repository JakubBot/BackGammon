
#include "ncurses.h"
#include "stdlib.h"
#include "stdio.h"
#include "../headers/menu.h"
#include "../headers/playerTurn.h"
#include "../headers/globalStructs.h"
#include "../headers/utils.h"
#include "../headers/move.h"
#include "../headers/bidirectionalList.h"
#include "../headers/renderGameUtils.h"
#include "../headers/fileActions.h"

void renderBoard(s_game *game, WINDOW *gameWin, int selectColumn, int targetColumn, struct Node *b_list);

void hideMenu()
{
    clrButtonPrints(19, 3);
}

void initializeColumns(s_game *game)
{
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
}

void printUpperCol(int offset_y, int offset_x, int x, int labelRowId, char *pawnLabel, int columnCount, char *label)
{
    int colElem = columnCount;
    x = x - (COLUMNS_COUNT / 2);

    for (int row = 0; row < BOARD_COLUMN_HEIGHT; ++row)
    {
        // normal chars
        int yOffset = row + offset_y;
        int xOffset = offset_x + (x * COLUMN_WIDTH);
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
    int labelPositionY = offset_y - 2;
    int labelPositionX = offset_x + (x * COLUMN_WIDTH);
    mvprintw(labelPositionY, offset_x + (x * COLUMN_WIDTH), "%d", labelRowId);

    if (colElem > 5)
    {
        mvprintw(labelPositionY - 1, labelPositionX - 1, "(%d)", colElem - 5);
    }
    else
    {
        mvprintw(labelPositionY - 1, labelPositionX - 1, "   ");
    }
}

void printBottomCol(int offset_y, int offset_x, int x, int labelRowId, char *pawnLabel, int columnCount, char *label, int count, int columnVerticalOffset)
{
    x = (COLUMNS_COUNT / 2) - x - 1;

    for (int row = 0; row < BOARD_COLUMN_HEIGHT; ++row)
    {
        int yOffset = row + offset_y + columnVerticalOffset + COLUMNGAP;
        int xOffset = offset_x + (x * COLUMN_WIDTH);
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
    int labelPositionY = BOARD_COLUMN_HEIGHT + offset_y + columnVerticalOffset + COLUMNGAP + 1;
    int labelPositionX = offset_x + (x * COLUMN_WIDTH);
    mvprintw(labelPositionY, offset_x + (x * COLUMN_WIDTH), "%d", labelRowId);
    if (columnCount > 5)
    {
        mvprintw(labelPositionY + 1, labelPositionX - 1, "(%d)", columnCount - 5);
    }
    else
    {
        mvprintw(labelPositionY + 1, labelPositionX - 1, "   ");
    }
}

void renderColumn(s_boardColumn boardColumn, int barOffset)
{
    int x = boardColumn.colX, rev = boardColumn.isReversed;
    // reversed 1 means that we want to reverse column
    int offset_y = 4;
    int offset_x = 1 + barOffset;
    int colVerticalOffset = COLUMN_WIDTH;
    char *label = (x % 2) == 1 ? rev ? "/\\" : "\\/" : "--";

    vector_t_pawn currCol = boardColumn.pawnIds;
    int colCount = currCol.count;
    char *p_label = currCol.ptr[0].color == 'b' ? BLACK_PAWN : WHITE_PAWN;
    int labelRowId = x + 1;

    int count = BOARD_COLUMN_HEIGHT - colCount;
    if (rev)
    {
        printBottomCol(offset_y, offset_x, x, labelRowId, p_label, colCount, label, count, colVerticalOffset);
    }
    else
    {
        printUpperCol(offset_y, offset_x, x, labelRowId, p_label, colCount, label);
    }
}

int getNextMoveCalculation(int firstVal, int secondVal, int whiteTurn)
{
    if (whiteTurn)
    {
        return firstVal + secondVal;
    }
    else
    {
        return firstVal - secondVal;
    }
}

int makeMove(struct Node **b_list, char action)
{
    if (action == 'l')
    {
        int valid = moveNext(b_list);
        if (!valid)
            return 0;
    }
    else if (action == 'r')
    {
        int valid = movePrev(b_list);
        if (!valid)
            return 0;
    }
    return 1;
}

int isValidNextMove(int move)
{

    if ((move < 0) || (move > (COLUMNS_COUNT - 1)))
    {
        return 0;
    }

    return 1;
}

int findNextLegalMove(s_game *game, char action, struct Node **b_list, int *currentActiveColumn, int *skippedColumns)
{
    int sourceColX = game->board.sourceColumn;
    int whiteTurn = isWhiteTurn(*game);

    struct Node *nextElement = NULL;

    if (action == 'l')
    {
        nextElement = next(*b_list);
        if (nextElement == NULL)
            return 0;
    }
    else
    {
        // action == 'r'
        nextElement = prev(*b_list);
        if (nextElement == NULL)
            return 0;
    }
    // else if (action == 'r')
    // {
    //     nextElement = prev(*b_list);
    //     if (nextElement == NULL)
    //         return 0;
    // }

    int step = nextElement->data;

    int move = getNextMoveCalculation(sourceColX, step, whiteTurn);
    int edge = (move == -1) || (move == COLUMNS_COUNT) ? 1 : 0;
    int pawnGoToHome = game->isPawnsHome && (game->removeFurthestPawn == 1 || edge) ? 1 : 0;
    // int pawnGoToHome = allPawnsHome(*game) && ((move == -1) || (move == COLUMNS_COUNT)) ? 1 : 0;

    if (pawnGoToHome)
    {
        game->board.pawnMoveToCourt = 1;

        *currentActiveColumn = move;

        int validMv = makeMove(b_list, action);

        return 0;
    }
    else
    {
        game->board.pawnMoveToCourt = 0;
    }

    int validNext = isValidNextMove(move);
    if (!validNext)
        return 0;

    int isValidCol = validPawnToColumnMove(game, move);

    if (!isValidCol)
    {
        int validMv = makeMove(b_list, action);
        *skippedColumns = *skippedColumns + 1;
        int res = findNextLegalMove(game, action, b_list, currentActiveColumn, skippedColumns);
        return res;
    }
    else
    {
        *skippedColumns = 0;
    }

    int validMv = makeMove(b_list, action);
    *currentActiveColumn = move;

    return 0;
}

int getDotsColor(s_game game, char activeTurn)
{
    int sourceColumn = game.board.sourceColumn;
    int forcedTargetColumn = game.board.forcedTargetColumn;

    int courtId = activeTurn == 'w' ? BLACK_COURT : WHITE_COURT;
    if ((sourceColumn == courtId && game.board.isBarActive) || (game.board.pawnMoveToCourt == 1 && getTurn(game) == activeTurn))
    {
        return COLOR_COLUMN_ID;
    }
    else if ((activeTurn == 'w' && getTurn(game) == 'w' && forcedTargetColumn == BLACK_COURT) || (activeTurn == 'b' && getTurn(game) == 'b' && forcedTargetColumn == WHITE_COURT))
    {
        return FORCED_COLUMN_MOVE_ID;
    }
    return NOT_FOUND;
}
void barActiveDots(s_game game)
{
    // additional square for start and end for each color
    // int clr = getClrPair(game.board.sourceColumn,game.board.isBarActive);
    int clrA = getDotsColor(game, 'w');
    if (clrA != NOT_FOUND)
    {

        attron(COLOR_PAIR(clrA));
        mvprintw(4, 53, SQUARE);
        attroff(COLOR_PAIR(clrA));
    }
    else
    {
        mvprintw(4, 53, " ");
    }

    int clrB = getDotsColor(game, 'b');
    if (clrB != NOT_FOUND)
    {
        attron(COLOR_PAIR(clrB));
        mvprintw(15, 53, SQUARE);
        attroff(COLOR_PAIR(clrB));
    }
    else
    {
        mvprintw(15, 53, " ");
    }

    refresh();
}

void setSelectedColumn(s_game *game, WINDOW *gameWin, struct Node *b_list, int currentActiveColumn, int selectColumn, int targetColumn)
{

    if (selectColumn)
    {
        game->board.sourceColumn = currentActiveColumn;
    }
    else if (targetColumn)
    {
        // game->board.pawnMoveToCourt

        if ((game->board.forcedTargetColumn != NOT_FOUND) && (currentActiveColumn != game->board.forcedTargetColumn))
        {
            moveToTop(&b_list);
            return renderBoard(game, gameWin, selectColumn, targetColumn, b_list);
        }
        else if ((currentActiveColumn == -1 || currentActiveColumn == 24) && game->board.pawnMoveToCourt != 1)
        {
            // block if pawn is on bar with source -1 or 24, he has to move it
            return renderBoard(game, gameWin, selectColumn, targetColumn, b_list);
        }

        if (game->diceInfo.isDoublet)
        {
            updateDiceDublet(game, b_list);
        }
        else
        {
            updateDice(game, b_list);
        }

        game->board.targetColumn = currentActiveColumn;

        clearSidebarInfo();
        showTurnInfo(*game);
    }
}

int handleBoardMove(s_game *game, struct Node **b_list, int selectColumn, int targetColumn, int *currentActiveColumn, int *skippedColumns)
{
    if (selectColumn == 1)
    {
        int ch = getch();
        curs_set(0);
        if (ch == KEY_LEFT)
        {
            findColumnWithPawn(*game, currentActiveColumn, 'l');
        }
        else if (ch == KEY_RIGHT)
        {
            findColumnWithPawn(*game, currentActiveColumn, 'r');
        }
        else if (ch == 10)
        {
            return 0;
        }
    }
    else if (targetColumn == 1)
    {
        int ch = getch();
        curs_set(0);
        if (ch == KEY_LEFT)
        {
            findNextLegalMove(game, 'l', b_list, currentActiveColumn, skippedColumns);
        }
        else if (ch == KEY_RIGHT)
        {
            findNextLegalMove(game, 'r', b_list, currentActiveColumn, skippedColumns);
        }
        else if (ch == 10)
        {
            return 0;
        }
    }
    return 1;
}

int getClrPair(int colX, int currentActiveColumn, int forcedTargetColumn)
{
    if (colX == currentActiveColumn)
    {
        return COLOR_COLUMN_ID;
    }
    else if (colX == forcedTargetColumn)
    {
        return FORCED_COLUMN_MOVE_ID;
    }
    else
    {
        return POSSIBLE_COLUMN_MOVE_ID;
    }
}

void printMainBoard(s_game game, int currentActiveColumn)
{

    for (int i = 0; i < COLUMNS_COUNT; ++i)
    {
        s_boardColumn boardColumn = game.board.columns[i];
        int barOffset = 0;
        if (boardColumn.colX < 6 || boardColumn.colX > 17)
        {
            barOffset = 4;
        }
        if ((boardColumn.colX == currentActiveColumn) || (boardColumn.colX == game.board.sourceColumn) || (boardColumn.colX == game.board.forcedTargetColumn))
        {
            int colorPair = getClrPair(boardColumn.colX, currentActiveColumn, game.board.forcedTargetColumn);
            // int colorPair = boardColumn.colX == currentActiveColumn ? 2 : 3;
            attron(COLOR_PAIR(colorPair));
            renderColumn(boardColumn, barOffset);

            attroff(COLOR_PAIR(colorPair));
        }
        else
        {
            renderColumn(boardColumn, barOffset);
        }
    }
}

void printBar(s_game game)
{
    int barPawnCount = getPawnBarCount(game);
    // int barElements = (MAX_PAWN_ON_COL * 2) + COLUMNGAP;
    for (int i = 1; i < BAR_COLUMN_HEIGHT; ++i)
    {
        mvprintw(i + COLUMNGAP, 1 + (6 * 4), "||");
    }
}

void printBarPawn(s_game game)
{
    int barPawnCount = getPawnBarCount(game);
    // int barElements = (MAX_PAWN_ON_COL * 2) + COLUMNGAP;
    vector_t_pawn *pawn = &game.board.bar.pawnIds;

    int start = (BAR_COLUMN_HEIGHT / 2) - (barPawnCount / 2);

    for (int i = 0; i < barPawnCount; ++i)
    {

        char clr = pawn->ptr[0].color;

        char *pawnLabel = clr == 'b' ? BLACK_PAWN : WHITE_PAWN;

        int pawnIndex = findPawnIndex(game.board.bar.pawnIds, game.turn);

        if (i == pawnIndex && game.board.isBarActive)
        {
            attron(COLOR_PAIR(2));
            mvprintw(i + COLUMNGAP + start, 1 + (6 * 4), pawnLabel);
            attroff(COLOR_PAIR(2));
        }
        else
        {
            mvprintw(i + COLUMNGAP + start, 1 + (6 * 4), pawnLabel);
        }
    }
}

void setDefaultActiveColumn(s_game game, int *currentActiveColumn, int targetColumn, int selectColumn)
{

    if (targetColumn && *currentActiveColumn == INITIAL_ACTIVE_COLUMN)
    {
        *currentActiveColumn = game.board.sourceColumn;
    }
    else if (selectColumn && *currentActiveColumn == INITIAL_ACTIVE_COLUMN)
    {
        findColumnWithPawn(game, currentActiveColumn, ' ');
    }
}

void renderBoard(s_game *game, WINDOW *gameWin, int selectColumn, int targetColumn, struct Node *b_list)
{
    // currentActiveColumn represents column(colX) that is active
    int currentActiveColumn = INITIAL_ACTIVE_COLUMN;

    int skippedColumns = 0;
    do
    {
        setDefaultActiveColumn(*game, &currentActiveColumn, targetColumn, selectColumn);

        barActiveDots(*game);

        printMainBoard(*game, currentActiveColumn);

        // render bar that is between 6 and 7 column
        printBar(*game);

        printBarPawn(*game);

        wrefresh(gameWin);

        int action = handleBoardMove(game, &b_list, selectColumn, targetColumn, &currentActiveColumn, &skippedColumns);
        if (action == 0)
            break;

    } while (selectColumn || targetColumn);

    for (int i = 0; i < skippedColumns; ++i)
    {
        movePrev(&b_list);
    }

    setSelectedColumn(game, gameWin, b_list, currentActiveColumn, selectColumn, targetColumn);
}

void getPossibleMovesArray(int moveArr[4], s_game game, int availableDiceMoves)
{
    int allNormalMovesAvailable = availableDiceMoves == 3;
    int isDoublet = game.diceInfo.isDoublet;

    int move1 = game.diceInfo.dice[0];
    int move2 = game.diceInfo.dice[1];

    int moveLeft = move1 == -1 ? move2 : move1;
    if (isDoublet)
    {
        for (int j = 0; j < availableDiceMoves; ++j)
        {
            moveArr[j] = game.diceInfo.dice[0] * (j + 1);
        }
    }
    else if (allNormalMovesAvailable)
    {
        moveArr[0] = move1;
        moveArr[1] = move2;
        moveArr[2] = move1 + move2;
    }
    else
    {
        moveArr[0] = moveLeft;
    }
}

int findPawn(int moves, int currentColIndex, int *moveArr, int whiteTurn, s_game game, int *souceTargetCapture)
{
    for (int k = 0; k < moves; ++k)
    {
        int nextColIndex = getNextMoveCalculation(currentColIndex, moveArr[k], whiteTurn);
        s_boardColumn nextCol = findColumnBasedOnColX(game, nextColIndex);
        int isValidNextCol = isValidNextMove(nextColIndex);
        if (!isValidNextCol)
            continue;

        if (nextCol.pawnIds.count == 1 && nextCol.pawnIds.ptr[0].color != game.turn)
        {
            souceTargetCapture[0] = currentColIndex;
            souceTargetCapture[1] = nextColIndex;

            return currentColIndex;
        }
    }
    return NOT_FOUND;
}

int hasFreeColumnToMove(int moves, int currentColIndex, int *moveArr, int whiteTurn, s_game game)
{
    for (int k = 0; k < moves; ++k)
    {
        int nextColIndex = getNextMoveCalculation(currentColIndex, moveArr[k], whiteTurn);
        // s_boardColumn nextCol = findColumnBasedOnColX(game, nextColIndex);

        int hasValidMove = validPawnToColumnMove(&game, nextColIndex);
        int pawnGoToHome = allPawnsHome(game) && ((nextColIndex <= WHITE_COURT) || (nextColIndex >= BLACK_COURT)) ? 1 : 0;
        int isValidNextCol = isValidNextMove(nextColIndex);

        if ((hasValidMove == 1 && isValidNextCol) || pawnGoToHome)
        {

            return currentColIndex;
        }
    }
    return NO_MOVE;
}

int hasNextLegalMove(s_game game)
{
    int useBarPawn = shouldUseBarPawn(game);

    char color = getTurn(game);
    int whiteTurn = color == 'w' ? 1 : 0;

    int isDoublet = game.diceInfo.isDoublet;
    int availableDiceMoves = game.diceInfo.availableDiceMoves;
    int allNormalMovesAvailable = availableDiceMoves == 3;

    int moves = isDoublet ? availableDiceMoves : allNormalMovesAvailable ? 3
                                                                         : 1;
    int moveArr[4];
    getPossibleMovesArray(moveArr, game, availableDiceMoves);

    if (useBarPawn)
    {

        int currentColIndex = whiteTurn ? WHITE_COURT : BLACK_COURT;
        int foundedIndex = hasFreeColumnToMove(moves, currentColIndex, moveArr, whiteTurn, game);

        if (foundedIndex != NO_MOVE)
            return foundedIndex;

        return NO_MOVE;
    }

    for (int currentColIndex = 0; currentColIndex < COLUMNS_COUNT; ++currentColIndex)
    {
        s_boardColumn currentColumn = findColumnBasedOnColX(game, currentColIndex);

        if (currentColumn.pawnIds.count == 0 || currentColumn.pawnIds.ptr[0].color != color)
        {
            continue;
        }

        int foundedIndex = hasFreeColumnToMove(moves, currentColIndex, moveArr, whiteTurn, game);

        if (foundedIndex != NO_MOVE)
            return foundedIndex;
    }

    return NO_MOVE;
}

int existsCapture(s_game game, struct Node *b_list, int *souceTargetCapture, int checkOnlyBar)
{
    char color = getTurn(game);
    int whiteTurn = color == 'w' ? 1 : 0;

    int isDoublet = game.diceInfo.isDoublet;
    int availableDiceMoves = game.diceInfo.availableDiceMoves;
    int allNormalMovesAvailable = availableDiceMoves == 3;

    int moves = isDoublet ? availableDiceMoves : allNormalMovesAvailable ? 3
                                                                         : 1;
    int moveArr[4];
    getPossibleMovesArray(moveArr, game, availableDiceMoves);

    if (checkOnlyBar)
    {

        int currentColIndex = whiteTurn ? WHITE_COURT : BLACK_COURT;
        int foundedIndex = findPawn(moves, currentColIndex, moveArr, whiteTurn, game, souceTargetCapture);
        if (foundedIndex != NOT_FOUND)
            return foundedIndex;

        return NOT_FOUND;
    }

    for (int currentColIndex = 0; currentColIndex < COLUMNS_COUNT; ++currentColIndex)
    {
        s_boardColumn currentColumn = findColumnBasedOnColX(game, currentColIndex);

        if (currentColumn.pawnIds.count == 0 || currentColumn.pawnIds.ptr[0].color != color)
        {
            continue;
        }
        int foundedIndex = findPawn(moves, currentColIndex, moveArr, whiteTurn, game, souceTargetCapture);
        if (foundedIndex != NOT_FOUND)
            return foundedIndex;
    }
    return NOT_FOUND;
}
int handleForcedCapture(s_game *game, struct Node *b_list, int checkOnlyBar)
{

    int souceTargetCapture[2];
    int existsCap = existsCapture(*game, b_list, souceTargetCapture, checkOnlyBar);

    if (existsCap != NOT_FOUND)
    {
        if (checkOnlyBar != 1)
        {
            game->board.sourceColumn = souceTargetCapture[0];
        }
        game->board.forcedTargetColumn = souceTargetCapture[1];
        return 1;
    }
    return 0;
}

int findFurthesPawn(s_game game)
{
    int whiteTurn = isWhiteTurn(game);
    int furthest = 0;
    int currentPosition = whiteTurn ? (COLUMNS_COUNT - 1) : 0;
    while ((currentPosition <= (COLUMNS_COUNT - 1)) && (currentPosition >= 0))
    {
        s_boardColumn currentColumn = findColumnBasedOnColX(game, currentPosition);

        if (currentColumn.pawnIds.count > 0 && currentColumn.pawnIds.ptr[0].color == game.turn)
        {
            furthest = currentPosition;
        }

        if (whiteTurn)
        {
            currentPosition--;
        }
        else
        {
            currentPosition++;
        }
    }
    return furthest;
}

int checkIfRemoveFurthestPawn(s_game *game)
{

    // min step
    int dice1 = game->diceInfo.dice[0];
    int dice2 = game->diceInfo.dice[1];
    int minStep = 0;

    if (dice1 == -1)
    {
        minStep = dice2;
    }
    else if (dice2 == -1)
    {
        minStep = dice1;
    }
    else
    {
        minStep = min(dice1, dice2);
    }
    int furthestPawn = findFurthesPawn(*game);
    int whiteTurn = isWhiteTurn(*game);

    int moveArr[4];
    int availableDiceMoves = game->diceInfo.availableDiceMoves;

    int isDoublet = game->diceInfo.isDoublet;
    int allNormalMovesAvailable = availableDiceMoves == 3;

    int moves = isDoublet ? availableDiceMoves : allNormalMovesAvailable ? 3
                                                                         : 1;
    getPossibleMovesArray(moveArr, *game, availableDiceMoves);

    int removeExcactDistancePawn = NOT_FOUND;

    for (int currentColIndex = 0; currentColIndex < COLUMNS_COUNT; ++currentColIndex)
    {

        if (removeExcactDistancePawn != NOT_FOUND)
            break;

        s_boardColumn nextCol = findColumnBasedOnColX(*game, currentColIndex);
        if (nextCol.pawnIds.count == 0 || nextCol.pawnIds.ptr[0].color != game->turn)
        {
            continue;
        }

        for (int k = 0; k < moves; ++k)
        {
            int nextColIndex = getNextMoveCalculation(currentColIndex, moveArr[k], whiteTurn);
            if (nextColIndex == BLACK_COURT || nextColIndex == WHITE_COURT)
            {
                removeExcactDistancePawn = currentColIndex;
                break;
            }
        }
    }
    int relativePawnPosition = whiteTurn ? (COLUMNS_COUNT - 1) - furthestPawn : furthestPawn;

    if (removeExcactDistancePawn != NOT_FOUND)
    {
        game->board.sourceColumn = removeExcactDistancePawn;
        if (whiteTurn)
        {
            game->board.forcedTargetColumn = BLACK_COURT;
        }
        else
        {
            game->board.forcedTargetColumn = WHITE_COURT;
        }
        return 1;
    }
    else if (minStep > relativePawnPosition)
    {
        game->board.sourceColumn = furthestPawn;
        game->removeFurthestPawn = 1;

        return 1;
    }

    return 0;
}

int setSourceColumn(s_game *game, WINDOW *gameWin, struct Node *b_list)
{

    if (shouldUseBarPawn(*game))
    {
        game->board.isBarActive = 1;
        if (isWhiteTurn(*game))
        {
            game->board.sourceColumn = -1;
        }
        else
        {
            game->board.sourceColumn = 24;
        }

        handleForcedCapture(game, b_list, 1);
        // int souceTargetCapture[2];
        // int existsCap = existsCapture(*game, b_list, souceTargetCapture, 1);

        // if (existsCap != NOT_FOUND)
        // {
        //     game->board.forcedTargetColumn = souceTargetCapture[1];
        // }
        return 1;
    }
    int hasForcedCapture = handleForcedCapture(game, b_list, 0);

    if (hasForcedCapture)
    {
        return 1;
    }
    // int souceTargetCapture[2];
    // int existsCap = existsCapture(*game, b_list, souceTargetCapture, 0);

    // if (existsCap != NOT_FOUND)
    // {
    //     game->board.sourceColumn = souceTargetCapture[0];
    //     game->board.forcedTargetColumn = souceTargetCapture[1];
    // }

    if (game->isPawnsHome)
    {
        int hasFurthestPawn = checkIfRemoveFurthestPawn(game);
        if (hasFurthestPawn)
        {
            return 1;
        }
    }

    game->board.isBarActive = 0;
    int srcIds[] = {5};
    // int srcIds[] = {5, 8};
    hideMenu();
    refresh();
    renderMenu(srcIds, sizeof(srcIds) / sizeof(srcIds[0]), 0, 0, 19, NULL, game);
    renderBoard(game, gameWin, 1, 0, NULL);
    return 1;
}

void moveRepeater(s_game *game, WINDOW *gameWin)
{
    struct Node *b_list = NULL;
    fillBList(&b_list, game);

    while (game->diceInfo.availableDiceMoves > 0)
    {
        int pawnsHome = allPawnsHome(*game);
        game->isPawnsHome = pawnsHome;

        int hasLegalMove = hasNextLegalMove(*game);
        if (hasLegalMove == NO_MOVE)
        {
            // show invalid move
            hideMenu();
            int invalidMvId[] = {9};
            renderMenu(invalidMvId, sizeof(invalidMvId) / sizeof(invalidMvId[0]), 0, 0, 19, NULL, game);
            break;
        }

        // source col
        setSourceColumn(game, gameWin, b_list);

        hideMenu();
        // target col
        int targetIds[] = {6};
        // int targetIds[] = {6, 8};
        renderMenu(targetIds, sizeof(targetIds) / sizeof(targetIds[0]), 0, 0, 19, NULL, game);
        renderBoard(game, gameWin, 0, 1, b_list);
        refresh();

        movePawn(game);

        renderBoard(game, gameWin, 0, 0, NULL);
    }
    freeList(&b_list);
}

void initializeGame(s_game *game)
{
    FILE *file = NULL;
    // FILE *file = fopen(CURRENT_GAME, "a");
    // fopen(CURRENT_GAME, "w");
    if (game->gameLoadedFromFile == 1)
    {
        file = fopen(CURRENT_GAME, "a");
    }
    else
    {
        file = fopen(CURRENT_GAME, "w");
    }
    if (file == NULL)
    {
        mvprintw(0, 0, "Error while opening file");
    }

    game->file = file;

    game->turn = 'w';
    game->initialDiceValueW = -1;
    game->initialDiceValueB = -1;

    game->board.sourceColumn = -1;
    game->board.targetColumn = -1;

    game->board.forcedTargetColumn = NOT_FOUND;
    vector_t_pawn *barPawn = &game->board.bar.pawnIds;

    init(barPawn);

    game->board.isBarActive = 0;

    game->removeFurthestPawn = 0;

    game->endGame.winner = 0;

    vector_t_pawn *courtPawns = &game->courtPawns;
    init(courtPawns);

    initializeColumns(game);

    //    for (int i = 0; i < PAWNS_COUNT; ++i)
    //    {
    //        game->wPawns[i].color = 'w';
    //        game->wPawns[i].id = i;
    //
    //        game->bPawns[i].color = 'b';
    //        game->bPawns[i].id = i;
    //    }
}

int checkGameEnd(s_game *game)
{
    int whitePawns = 0;
    int blackPawns = 0;
    vector_t_pawn courtPawns = game->courtPawns;

    for (int i = 0; i < courtPawns.count; ++i)
    {
        if (courtPawns.ptr[i].color == 'w')
        {
            whitePawns++;
        }
        else
        {
            blackPawns++;
        }
    }
    int winner = -1;
    if (whitePawns == 15)
    {
        winner = 1;
    }
    else if (blackPawns == 15)
    {
        winner = 2;
    }

    if (winner != -1)
    {
        game->endGame.winner = winner;
        int gameEndedId[] = {10};
        hideMenu();
        renderMenu(gameEndedId, sizeof(gameEndedId) / sizeof(gameEndedId[0]), 0, 0, 19, NULL, game);
        return 1;
    }
    return 0;
}

void gameLoop(s_game game, WINDOW *gameWin)
{

    while (game.endGame.winner == 0)
    {

        // this will render menu with roll dice option
        int menuIds[] = {4, 8};
        int diceSize = 0;
        hideMenu();
        renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), 0, 0, 19, &diceSize, &game);

        curs_set(0);

        moveRepeater(&game, gameWin);

        changeTurn(&game);

        refresh();
        wrefresh(gameWin);
        clearSidebarInfo();

        saveCurrentState(game);

        checkGameEnd(&game);
    }

    // gameLoop(game, gameWin);
}

void initGame()
{
    while (1)
    {
        int menuIds[] = {0, 1, 2, 3};
        renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), 0, 0, 0, NULL, NULL);
    }
    // initGame();
}

void renderGame(int loadFromFile)
{

    erase();
    refresh();

    mvprintw(0, 0, "Backgammon");
    s_game game = {};
    game.gameLoadedFromFile = loadFromFile == 1 ? 1 : 0;

    initializeGame(&game);

    if (loadFromFile == 1)
    {
        updateGameFile(1, game);
        loadFile(&game);
    }

    int yStart = 3, xStart = 0;
    WINDOW *gameWin = newwin(BOARD_HEIGHT, BOARD_WIDTH, yStart, xStart);
    wrefresh(gameWin);
    box(gameWin, 0, 0);
    wrefresh(gameWin);
    renderBoard(&game, gameWin, 0, 0, NULL);

    refresh();
    if (loadFromFile == 0)
    {
        updateInitialDiceValues(&game);
    }

    wrefresh(gameWin);

    gameLoop(game, gameWin);

    if (game.file != NULL)
    {
        fclose(game.file);
    }
}
