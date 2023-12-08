
#include "ncurses.h"
#include "stdlib.h"
#include "stdio.h"
#include "../headers/globalStructs.h"
#include "../headers/menu.h"
#include "../headers/playerTurn.h"
#include "../headers/utils.h"
#include "../headers/move.h"
#include "../headers/bidirectionalList.h"
#include "../headers/renderGameUtils.h"
#include "../headers/fileActions.h"
#include "../headers/hallOfFame.h"

void renderBoard(s_game *game, WINDOW *gameWin, int selectColumn, int targetColumn, struct Node *b_list);

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

void printUpperCol(int offset_y, int offset_x, int x, int labelRowId, char *pawnLabel, int colCount, char *label)
{
    int col = colCount;
    x = x - (COLUMNS_COUNT / 2);

    for (int row = 0; row < BOARD_COLUMN_HEIGHT; ++row)
    {
        // normal chars
        int yOffset = row + offset_y;
        int xOffset = offset_x + (x * COLUMN_WIDTH);
        if (colCount > 0)
        {
            mvprintw(yOffset, xOffset, pawnLabel);
            colCount--;
        }
        else
        {
            mvprintw(yOffset, xOffset, label);
        }
    }

    printUpperColAdditionalPawns(offset_y, offset_x, x, labelRowId, col);
}

void printBottomCol(int offset_y, int offset_x, int x, int labelRowId, char *pawnLabel, int colCount, char *label, int count, int verticalOffset)
{
    x = (COLUMNS_COUNT / 2) - x - 1;

    for (int row = 0; row < BOARD_COLUMN_HEIGHT; ++row)
    {
        int yOffset = row + offset_y + verticalOffset + COLUMNGAP;
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

    printBottomColAdditionalPawns(offset_y, offset_x, x, labelRowId, colCount, verticalOffset);
}

void renderColumn(s_boardColumn boardColumn, int barOffset)
{
    int x = boardColumn.colX, rev = boardColumn.isReversed;
    // reversed 1 means that we want to reverse column
    int offset_y = 4;
    int offset_x = 1 + barOffset;
    char *label = (x % 2) == 1 ? rev ? "/\\" : "\\/" : "--";

    vector_t_pawn currCol = boardColumn.pawnIds;
    int colCount = currCol.count;
    char *p_label = currCol.ptr[0].color == 'b' ? BLACK_PAWN : WHITE_PAWN;
    int labelRowId = x + 1;

    int count = BOARD_COLUMN_HEIGHT - colCount;
    if (rev)
    {
        printBottomCol(offset_y, offset_x, x, labelRowId, p_label, colCount, label, count, COLUMN_WIDTH);
    }
    else
    {
        printUpperCol(offset_y, offset_x, x, labelRowId, p_label, colCount, label);
    }
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
    int clrA = getDotsColor(game, 'w');
    if (clrA != NOT_FOUND)
    {
        attron(COLOR_PAIR(clrA));
        mvprintw(DOT_X_W, DOT_Y, SQUARE);
        attroff(COLOR_PAIR(clrA));
    }
    else
    {
        mvprintw(DOT_X_W, DOT_Y, " ");
    }

    int clrB = getDotsColor(game, 'b');
    if (clrB != NOT_FOUND)
    {
        attron(COLOR_PAIR(clrB));
        mvprintw(DOT_X_B, DOT_Y, SQUARE);
        attroff(COLOR_PAIR(clrB));
    }
    else
    {
        mvprintw(DOT_X_B, DOT_Y, " ");
    }

    refresh();
}

void updateDiceInfo(s_game *game, struct Node *b_list)
{
    if (game->diceInfo.isDoublet)
    {
        updateDiceDublet(game, b_list);
    }
    else
    {
        updateDice(game, b_list);
    }
}

int handleReRenderBoardOnIllegalMove(s_game *game, WINDOW *gameWin, struct Node *b_list, int activeColumn, int selectColumn, int targetColumn)
{
    if ((game->board.forcedTargetColumn != NOT_FOUND) && (activeColumn != game->board.forcedTargetColumn))
    {
        moveToTop(&b_list);
        renderBoard(game, gameWin, selectColumn, targetColumn, b_list);
        return 1;
    }
    else if ((activeColumn == WHITE_COURT || activeColumn == BLACK_COURT) && game->board.pawnMoveToCourt != 1)
    {
        // block if pawn is on bar with source -1 or 24, he has to move it
        renderBoard(game, gameWin, selectColumn, targetColumn, b_list);
        return 1;
    }
    return 0;
}

void setSelectedColumn(s_game *game, WINDOW *gameWin, struct Node *b_list, int activeColumn, int selectColumn, int targetColumn)
{
    if (selectColumn)
    {
        game->board.sourceColumn = activeColumn;
    }
    else if (targetColumn)
    {
        int reRender = handleReRenderBoardOnIllegalMove(game, gameWin, b_list, activeColumn, selectColumn, targetColumn);
        if (reRender)
            return;

        updateDiceInfo(game, b_list);

        game->board.targetColumn = activeColumn;

        clearSidebarInfo();

        showTurnInfo(*game);
    }
}

int handleBoardMove(s_game *game, struct Node **b_list, int selectColumn, int targetColumn, int *activeColumn, int *skippedColumns)
{
    if (selectColumn == 1)
    {
        return handleBoardMoveLeft(*game, activeColumn);
    }
    else if (targetColumn == 1)
    {
        return handleBoardMoveRight(game, activeColumn, b_list, skippedColumns);
    }
    return 1;
}

void printMainBoard(s_game game, int activeColumn)
{
    for (int i = 0; i < COLUMNS_COUNT; ++i)
    {
        s_boardColumn boardColumn = game.board.columns[i];
        int colX = boardColumn.colX;

        int barOffset = 0;
        if (colX < 6 || colX > 17)
        {
            barOffset = 4;
        }
        if ((colX == activeColumn) || (colX == game.board.sourceColumn) || (colX == game.board.forcedTargetColumn))
        {
            int colorPair = getClrPair(colX, activeColumn, game.board.forcedTargetColumn);
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

    for (int i = 1; i < BAR_COLUMN_HEIGHT; ++i)
    {
        mvprintw(i + COLUMNGAP, 1 + (6 * 4), "||");
    }
}

void printBarPawn(s_game game)
{
    int barPawnCount = getPawnBarCount(game);
    vector_t_pawn *pawn = &game.board.bar.pawnIds;

    int start = (BAR_COLUMN_HEIGHT / 2) - (barPawnCount / 2);

    for (int i = 0; i < barPawnCount; ++i)
    {

        char clr = pawn->ptr[i].color;

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

void setDefaultActiveColumn(s_game game, int *activeColumn, int targetColumn, int selectColumn)
{
    if (targetColumn && *activeColumn == INITIAL_ACTIVE_COLUMN)
    {
        *activeColumn = game.board.sourceColumn;
    }
    else if (selectColumn && *activeColumn == INITIAL_ACTIVE_COLUMN)
    {
        findColumnWithPawn(game, activeColumn, ' ');
    }
}

void printBoard(s_game game, WINDOW *gameWin, int activeColumn)
{
    barActiveDots(game);

    printMainBoard(game, activeColumn);

    printBar(game);

    printBarPawn(game);

    wrefresh(gameWin);
}

void renderBoard(s_game *game, WINDOW *gameWin, int selectColumn, int targetColumn, struct Node *b_list)
{
    int activeColumn = INITIAL_ACTIVE_COLUMN;

    int skipCol = 0;
    do
    {
        setDefaultActiveColumn(*game, &activeColumn, targetColumn, selectColumn);

        printBoard(*game, gameWin, activeColumn);
        // barActiveDots(*game);

        // printMainBoard(*game, activeColumn);

        // printBar(*game);

        // printBarPawn(*game);

        // wrefresh(gameWin);

        int action = handleBoardMove(game, &b_list, selectColumn, targetColumn, &activeColumn, &skipCol);
        if (action == 0)
            break;

    } while (selectColumn || targetColumn);

    for (int i = 0; i < skipCol; ++i)
    {
        movePrev(&b_list);
    }

    setSelectedColumn(game, gameWin, b_list, activeColumn, selectColumn, targetColumn);
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
        moveArr[1] = 0;
        moveArr[2] = 0;
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

int handlePawnLegalMv(s_game game, int *moveArr, int whiteTurn, int moves, int currentColIndex)
{
    int foundedIndex = hasFreeColumnToMove(moves, currentColIndex, moveArr, whiteTurn, game);

    if (foundedIndex != NO_MOVE)
        return foundedIndex;

    return NO_MOVE;
}

int checkValidPosition(s_game game, int useBarPawn, int whiteTurn, int *moveArr, int moves, char clr)
{
    if (useBarPawn)
    {
        int colIndex = whiteTurn ? WHITE_COURT : BLACK_COURT;

        return handlePawnLegalMv(game, moveArr, whiteTurn, moves, colIndex);
    }

    for (int colIndex = 0; colIndex < COLUMNS_COUNT; ++colIndex)
    {
        s_boardColumn currentColumn = findColumnBasedOnColX(game, colIndex);

        if (checkGameTurnPawns(currentColumn, clr))
        {
            continue;
        }

        int legalMv = handlePawnLegalMv(game, moveArr, whiteTurn, moves, colIndex);
        if (legalMv != NO_MOVE)
        {
            return legalMv;
        }
    }
    return NO_MOVE;
}

s_diceContainer getDiceData(s_game game)
{
    s_diceContainer diceContainer = {};

    char color = getTurn(game);
    int whiteTurn = color == 'w' ? 1 : 0;
    int isDoublet = game.diceInfo.isDoublet;
    int availableDiceMoves = game.diceInfo.availableDiceMoves;
    int allNormalMovesAvailable = availableDiceMoves == 3;

    int moves = isDoublet ? availableDiceMoves : allNormalMovesAvailable ? 3
                                                                         : 1;

    // diceContainer.isDoublet = game.diceInfo.isDoublet;
    // diceContainer.availableDiceMoves = availableDiceMoves;
    // diceContainer.allNormalMovesAvailable = allNormalMovesAvailable;
    // int moveArr[4];
    // getPossibleMovesArray(moveArr, game, availableDiceMoves);

    diceContainer.color = color;
    diceContainer.whiteTurn = whiteTurn;

    diceContainer.moves = moves;

    getPossibleMovesArray(diceContainer.moveArr, game, availableDiceMoves);

    return diceContainer;
}

int hasNextLegalMove(s_game game)
{
    int useBarPawn = shouldUseBarPawn(game);

    // char color = getTurn(game);
    // int whiteTurn = color == 'w' ? 1 : 0;

    // int isDoublet = game.diceInfo.isDoublet;
    // int availableDiceMoves = game.diceInfo.availableDiceMoves;
    // int allNormalMovesAvailable = availableDiceMoves == 3;

    // int moves = isDoublet ? availableDiceMoves : allNormalMovesAvailable ? 3
    //                                                                      : 1;
    // int moveArr[4];
    // getPossibleMovesArray(moveArr, game, availableDiceMoves);
    s_diceContainer diceInfo = getDiceData(game);
    // if (useBarPawn)
    // {
    //     int currentColIndex = whiteTurn ? WHITE_COURT : BLACK_COURT;

    //     return handlePawnLegalMv(game, moveArr, whiteTurn, moves, currentColIndex);
    // }

    // for (int currentColIndex = 0; currentColIndex < COLUMNS_COUNT; ++currentColIndex)
    // {
    //     s_boardColumn currentColumn = findColumnBasedOnColX(game, currentColIndex);

    //     if (currentColumn.pawnIds.count == 0 || currentColumn.pawnIds.ptr[0].color != color)
    //     // if (checkGameTurnPawns(currentColumn, color))
    //     {
    //         continue;
    //     }

    //     // int handlePawnLegalMv(s_game game, int *moveArr, int whiteTurn, int moves, int currentColIndex)
    //     // {
    //     //     int foundedIndex = hasFreeColumnToMove(moves, currentColIndex, moveArr, whiteTurn, game);

    //     //     if (foundedIndex != NO_MOVE)
    //     //         return foundedIndex;

    //     //     return NO_MOVE;
    //     // }

    //     int legalMv = handlePawnLegalMv(game, moveArr, whiteTurn, moves, currentColIndex);
    //     if (legalMv != NO_MOVE)
    //     {
    //         return legalMv;
    //     }
    //     // int foundedIndex = hasFreeColumnToMove(moves, currentColIndex, moveArr, whiteTurn, game);

    //     // if (foundedIndex != NO_MOVE)
    //     //     return foundedIndex;
    // }
    // // return NO_MOVE;
    return checkValidPosition(game, useBarPawn, diceInfo.whiteTurn, diceInfo.moveArr, diceInfo.moves, diceInfo.color);
}

int getCourt(int whiteT)
{
    return whiteT ? WHITE_COURT : BLACK_COURT;
}

int existsCapture(s_game game, int *captureData, int checkBar)
{
    s_diceContainer dice = getDiceData(game);

    int white = dice.whiteTurn;
    // int isDoublet = game.diceInfo.isDoublet;
    // int availableDiceMoves = game.diceInfo.availableDiceMoves;
    // int allNormalMovesAvailable = availableDiceMoves == 3;

    // int moves = isDoublet ? availableDiceMoves : allNormalMovesAvailable ? 3
    //                                                                      : 1;
    // int moveArr[4];
    // getPossibleMovesArray(moveArr, game, availableDiceMoves);

    if (checkBar)
    {
        int pIdx = findPawn(dice.moves, getCourt(white), dice.moveArr, white, game, captureData);
        if (pIdx != NOT_FOUND)
            return pIdx;

        return NOT_FOUND;
    }

    int i = white ? 0 : COLUMNS_COUNT - 1;
    while (i < COLUMNS_COUNT && i >= 0)
    {
        s_boardColumn col = findColumnBasedOnColX(game, i);

        if (!checkGameTurnPawns(col, dice.color))
        {
            // continue;
            int pIdx = findPawn(dice.moves, i, dice.moveArr, white, game, captureData);
            if (pIdx != NOT_FOUND)
                return pIdx;
        }

        if (white)
            i++;
        else
            i--;
    }

    return NOT_FOUND;
}

int handleForcedCapture(s_game *game, struct Node *b_list, int checkOnlyBar)
{
    int souceTargetCapture[2];
    int existsCap = existsCapture(*game, souceTargetCapture, checkOnlyBar);

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

int setDistancePawnSource(s_game *game, int minStep, int removeExcactDistancePawn)
{
    int furthestPawn = findFurthesPawn(*game);
    int whiteTurn = game->turn == 'w' ? 1 : 0;
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

int checkIfRemoveFurthestPawn(s_game *game)
{

    int minStep = getCurrentMinDiceVal(*game);

    s_diceContainer diceC = getDiceData(*game);

    int whiteTurn = diceC.whiteTurn;

    int distancePawn = NOT_FOUND;

    for (int colIdx = 0; colIdx < COLUMNS_COUNT; ++colIdx)
    {
        if (distancePawn != NOT_FOUND)
            break;

        s_boardColumn col = findColumnBasedOnColX(*game, colIdx);

        // if (nextCol.pawnIds.count == 0 || nextCol.pawnIds.ptr[0].color != game->turn)
        if (checkGameTurnPawns(col, diceC.color))
        {
            continue;
        }

        distancePawn = checkCourtEnter(game, colIdx, diceC.moveArr, diceC.moves, whiteTurn);
    }

    int hasDistancePawn = setDistancePawnSource(game, minStep, distancePawn);
    if (hasDistancePawn)
        return 1;

    return 0;
}

int setSourceColumn(s_game *game, WINDOW *gameWin, struct Node *b_list)
{

    if (shouldUseBarPawn(*game))
    {
        game->board.isBarActive = 1;
        if (isWhiteTurn(*game))
        {
            game->board.sourceColumn = WHITE_COURT;
        }
        else
        {
            game->board.sourceColumn = BLACK_COURT;
        }

        handleForcedCapture(game, b_list, 1);

        return 1;
    }
    int hasForcedCapture = handleForcedCapture(game, b_list, 0);

    if (hasForcedCapture)
    {
        return 1;
    }

    if (game->isPawnsHome)
    {
        int hasFurthestPawn = checkIfRemoveFurthestPawn(game);
        if (hasFurthestPawn)
        {
            return 1;
        }
    }

    // game->board.isBarActive = 0;
    // int srcIds[] = {5};

    // hideMenu();
    // refresh();
    // renderMenu(srcIds, sizeof(srcIds) / sizeof(srcIds[0]), 0, 0, 19, NULL, game);
    // renderBoard(game, gameWin, 1, 0, NULL);
    printSelectPawn(game, gameWin);
    return 1;
}

int noPawnsOnBoard(s_game game)
{
    int turn = getTurn(game);

    for (int i = 0; i < COLUMNS_COUNT; ++i)
    {
        s_boardColumn currentCol = findColumnBasedOnColX(game, i);
        vector_t_pawn pawn = currentCol.pawnIds;
        if (pawn.count > 0 && pawn.ptr[0].color == turn)
        {
            return 0;
        }
    }

    vector_t_pawn barPawn = game.board.bar.pawnIds;
    for (int i = 0; i < barPawn.count; ++i)
    {
        if (barPawn.ptr[i].color == turn)
        {
            return 0;
        }
    }

    return 1;
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
    if (whitePawns == 15 || noPawnsOnBoard(*game))
    {
        winner = 1;
    }
    else if (blackPawns == 15 || noPawnsOnBoard(*game))
    {
        winner = 2;
    }

    if (winner != -1)
    {
        game->endGame.winner = winner;
        int gameEndedId[] = {10};
        // hideMenu();
        clrButtonPrints(2, 30);
        refresh();
        updateWinnerScore(game);
        showEndGameInfo(*game);
        renderMenu(gameEndedId, sizeof(gameEndedId) / sizeof(gameEndedId[0]), 0, 0, 19, NULL, game);

        return 1;
    }
    return 0;
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
            showInvalidMenu(game);
            break;
        }

        handleSourceTargetMove(game, gameWin, b_list);

        movePawn(game);

        renderBoard(game, gameWin, 0, 0, b_list);

        int gameEnd = checkGameEnd(game);
        if (gameEnd == 1)
            break;
    }

    if (b_list != NULL)
    {
        moveToTop(&b_list);
        freeList(&b_list);

        b_list = NULL;
    }
}

void initializeGame(s_game *game)
{

    initializeFile(game);
    game->turn = 'w';
    game->initialDiceValueW = -1;
    game->initialDiceValueB = -1;

    initializeBoardData(game);

    game->removeFurthestPawn = 0;

    game->endGame.winner = 0;

    game->currentMenuBtnIndex = 0;

    game->usersData.firstPlayerSelects = 1;

    initializeGameReview(game);

    vector_t_pawn *barPawn = &game->board.bar.pawnIds;
    init(barPawn);

    vector_t_pawn *courtPawns = &game->courtPawns;
    init(courtPawns);

    initializeColumns(game);
}

void gameLoop(s_game game, WINDOW *gameWin)
{

    // initial position
    saveCurrentState(game);
    while (game.endGame.winner == 0)
    {
        int gameEnded = checkGameEnd(&game);
        if (gameEnded == 1)
        {
            break;
        }
        // this will render menu with roll dice option
        int menuIds[] = {4, 8};
        int diceSize = 0;
        hideMenu();

        // w tym jest error
        renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), 0, 0, 19, &diceSize, &game);
        curs_set(0);

        moveRepeater(&game, gameWin);

        changeTurn(&game);

        refresh();
        wrefresh(gameWin);
        clearSidebarInfo();

        saveCurrentState(game);
    }
}

void initGame()
{
    while (1)
    {
        clear();
        refresh();

        int menuIds[] = {0, 1, 2, 11, 3};
        renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), 0, 0, 0, NULL, NULL);
    }
}

WINDOW *boardInitialization(s_game *game)
{
    int yStart = 3, xStart = 0;
    WINDOW *gameWin = newwin(BOARD_HEIGHT, BOARD_WIDTH, yStart, xStart);
    wrefresh(gameWin);
    box(gameWin, 0, 0);
    wrefresh(gameWin);
    renderBoard(game, gameWin, 0, 0, NULL);

    refresh();
    wrefresh(gameWin);

    return gameWin;
}

void gameReview()
{
    erase();
    refresh();
    mvprintw(0, 0, "Review Your Game");

    s_game game = {};
    initializeGame(&game);

    updateGameFile(1, game);

    WINDOW *gameWin = boardInitialization(&game);

    FILE *file = fopen(CURRENT_GAME, "r");
    int fileLength = countLines(file);
    game.gameReview.fileLength = fileLength;

    showGameReviewOptions(&game, gameWin);

    delwin(gameWin);
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
        loadFile(&game, FILE_LAST_LINE);
    }
    curs_set(0);

    selectsUserAccount(&game);

    WINDOW *gameWin = boardInitialization(&game);

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
    delwin(gameWin);

    if (game.endGame.winner)
    {
    }
}
