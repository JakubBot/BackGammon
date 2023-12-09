
#include "../headers/globalStructs.h"
#include "../headers/utils.h"
#include "../headers/bidirectionalList.h"
#include "../headers/fileActions.h"
#include "../headers/menu.h"
#include "../headers/renderGame.h"

int findNextLegalMove(s_game *game, char action, struct Node **b_list, int *currentActiveColumn, int *skippedColumns);

void hideMenu()
{
    clrButtonPrints(19, 3);
}

void clearGameState(s_game *game)
{
    fclose(game->file);
    updateGameFile(0, *game);
    // ,struct Node** b_list

    // clear board

    // clear bidirectional list
    //  freeList(b_list);

    // remove allocated memory for vector pawn ids

    cleanup(&game->board.bar.pawnIds);
    cleanup(&game->courtPawns);

    for (int i = 0; i < COLUMNS_COUNT; ++i)
    {
        cleanup(&game->board.columns[i].pawnIds);
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
            if ((game.turn == 'w' && color == 'w') || (game.turn == 'b' && color == 'b'))
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

int shouldUseBarPawn(s_game game)
{
    if (game.board.bar.pawnIds.count == 0)
        return 0;

    for (int i = 0; i < game.board.bar.pawnIds.count; ++i)
    {
        if (game.board.bar.pawnIds.ptr[i].color == game.turn)
        {
            return 1;
        }
    }
    return 0;
}

void fillBList(struct Node **b_list, s_game *game)
{
    if (game == NULL)
        return;

    int *dice = game->diceInfo.dice;
    // initial value
    append(b_list, 0);
    if (game->diceInfo.isDoublet)
    {
        for (int i = 0; i < game->diceInfo.diceSize; ++i)
        {
            append(b_list, dice[0] * (i + 1));
        }
    }
    else
    {
        int dice1 = dice[0];
        int dice2 = dice[1];

        append(b_list, min(dice1, dice2));
        append(b_list, max(dice1, dice2));

        append(b_list, dice1 + dice2);
    }
}

void addPawns(vector_t_pawn *boardColumn, int id, char c)
{
    push_back(boardColumn, (s_pawn){.id = id, .color = c});
}

void getInitialColumnsIds(vector_t_pawn *bColumn, int col)
{
    char c = col == 0 || col == 11 || col == 16 || col == 18 ? 'b' : 'w';
    if (col == 11 || 23 == col)
    {
        addPawns(bColumn, 0, c);
        addPawns(bColumn, 1, c);
    }
    else if (col == 0 || col == 12)
    {
        addPawns(bColumn, 2, c);
        addPawns(bColumn, 3, c);
        addPawns(bColumn, 4, c);
        addPawns(bColumn, 5, c);
        addPawns(bColumn, 6, c);
    }
    else if (col == 4 || col == 16)
    {
        addPawns(bColumn, 12, c);
        addPawns(bColumn, 13, c);
        addPawns(bColumn, 14, c);
    }
    else if (col == 6 || col == 18)
    {
        addPawns(bColumn, 7, c);
        addPawns(bColumn, 8, c);
        addPawns(bColumn, 9, c);
        addPawns(bColumn, 10, c);
        addPawns(bColumn, 11, c);
    }
}

int getMoveDicePosition(struct Node *b_list, s_game game)
{

    int moveIndex = countDepthRecursive(b_list);
    if (moveIndex == 3 || moveIndex == 0)
        return moveIndex;
    int val = b_list->data;
    int position = 0;
    for (int i = 0; i < game.diceInfo.diceSize; ++i)
    {
        if (game.diceInfo.dice[i] == val)
        {
            position = i + 1;
            break;
        }
    }
    return position;
}

void updateDiceDublet(s_game *game, struct Node *b_list)
{
    if (countDepthRecursive(b_list) != 0)
    {
        int doubletMove = b_list->data;
        int moveIndex = doubletMove / game->diceInfo.dice[0];
        int moveDiff = game->diceInfo.availableDiceMoves - moveIndex;
        game->diceInfo.availableDiceMoves = moveDiff;

        for (int i = 4; i > game->diceInfo.availableDiceMoves; --i)
        {
            game->diceInfo.dice[i - 1] = -1;
        }

        if (b_list != NULL)
        {

            moveToTop(&b_list);

            for (int i = 0; i < moveIndex; ++i)
            {
                // removeLastNode(&b_list);
                b_list = removeLastNode(b_list);
            }
        }
    }
}

void updateDice(s_game *game, struct Node *b_list)
{

    int moveIndex = getMoveDicePosition(b_list, *game);

    if (moveIndex != 0)
    {
        game->diceInfo.availableDiceMoves = game->diceInfo.availableDiceMoves - moveIndex;

        // int dice1 = game->diceInfo.dice[10];
        int dice1 = game->diceInfo.dice[0];
        int dice2 = game->diceInfo.dice[1];

        if (dice1 != -1 && dice2 != -1 && moveIndex != 3)
        {
            if (b_list != NULL)
            {
                moveToTop(&b_list);
                b_list = removeLastNode(b_list);
            }
        }

        int diceVal = game->diceInfo.dice[moveIndex - 1];
        game->diceInfo.dice[moveIndex - 1] = -1;

        removeFirstNodeWithValue(&b_list, diceVal);
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

void showGameReviewOptions(s_game *game, WINDOW *gameWin)
{

    curs_set(0);

    int menuIds[] = {12, 13, 14, 15, 16};
    hideMenu();
    while (1)
    {

        renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), game->currentMenuBtnIndex, 0, 19, NULL, game);
        if (game->gameReview.exitGameReview == 1)
        {
            break;
        }
        int currentPosition = game->gameReview.currentFilePosition;
        loadFile(game, currentPosition);

        renderBoard(game, gameWin, 0, 0, NULL);
        refresh();
        wrefresh(gameWin);
    }
}

void initializeFile(s_game *game)
{
    FILE *file = NULL;
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
}

void initializeGameReview(s_game *game)
{
    game->gameReview.exitGameReview = 0;
    game->gameReview.currentFilePosition = 1;
    game->gameReview.fileLength = 0;
}

void initializeBoardData(s_game *game)
{
    game->board.sourceColumn = -1;
    game->board.targetColumn = -1;

    game->board.forcedTargetColumn = NOT_FOUND;

    game->board.isBarActive = 0;
}

void showInvalidMenu(s_game *game)
{
    hideMenu();
    int invalidMvId[] = {9};
    renderMenu(invalidMvId, sizeof(invalidMvId) / sizeof(invalidMvId[0]), 0, 0, 19, NULL, game);
}

void handleSourceTargetMove(s_game *game, WINDOW *gameWin, struct Node *b_list)
{
    // source col
    setSourceColumn(game, gameWin, b_list);

    hideMenu();
    // target col
    int targetIds[] = {6};
    renderMenu(targetIds, sizeof(targetIds) / sizeof(targetIds[0]), 0, 0, 19, NULL, game);
    renderBoard(game, gameWin, 0, 1, b_list);
    refresh();
}

void printUpperColAdditionalPawns(int offset_y, int offset_x, int x, int labelRowId, int col)
{
    int _y = offset_y - 2;
    int _x = offset_x + (x * COLUMN_WIDTH);
    mvprintw(_y, _x, "%d", labelRowId);

    if (col > 5)
        mvprintw(_y - 1, _x - 1, "(%d)", col - 5);
    else
        mvprintw(_y - 1, _x - 1, "   ");
}

void printBottomColAdditionalPawns(int offset_y, int offset_x, int x, int labelRowId, int colCount, int verticalOffset)
{
    int _y = BOARD_COLUMN_HEIGHT + offset_y + verticalOffset + COLUMNGAP + 1;
    int _x = offset_x + (x * COLUMN_WIDTH);
    mvprintw(_y, _x, "%d", labelRowId);
    if (colCount > 5)
    {
        mvprintw(_y + 1, _x - 1, "(%d)", colCount - 5);
    }
    else
    {
        mvprintw(_y + 1, _x - 1, "   ");
    }
}

int handleBoardMoveLeft(s_game game, int *currentActiveColumn)
{
    int ch = getch();
    curs_set(0);
    if (ch == KEY_LEFT)
    {
        findColumnWithPawn(game, currentActiveColumn, 'l');
    }
    else if (ch == KEY_RIGHT)
    {
        findColumnWithPawn(game, currentActiveColumn, 'r');
    }
    else if (ch == 10)
    {
        return 0;
    }
    return 1;
}
int handleBoardMoveRight(s_game *game, int *currentActiveColumn, struct Node **b_list, int *skippedColumns)
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
    return 1;
}

int isEdge(int move)
{
    return (move == -1) || (move == COLUMNS_COUNT) ? 1 : 0;
}

int handlePawnToHome(s_game *game, int move, int *currentActiveColumn, char action, struct Node **b_list)
{
    int edge = isEdge(move);

    int pawnGoToHome = game->isPawnsHome && (game->removeFurthestPawn == 1 || edge) ? 1 : 0;
    if (pawnGoToHome)
    {
        game->board.pawnMoveToCourt = 1;

        *currentActiveColumn = move;

        makeMove(b_list, action);

        return 1;
    }
    else
    {
        game->board.pawnMoveToCourt = 0;
    }
    return 0;
}

int validateNextMove(s_game *game, int move, int *currentActiveColumn, int *skippedColumns, char action, struct Node **b_list)
{

    int goHome = handlePawnToHome(game, move, currentActiveColumn, action, b_list);
    if (goHome == 1)
        return 0;

    int validNext = isValidNextMove(move);
    if (!validNext)
        return 0;

    int isValidCol = validPawnToColumnMove(game, move);

    if (!isValidCol)
    {
        makeMove(b_list, action);
        *skippedColumns = *skippedColumns + 1;
        int res = findNextLegalMove(game, action, b_list, currentActiveColumn, skippedColumns);
        return res;
    }
    else
    {
        *skippedColumns = 0;
    }
    return 1;
}

struct Node *getListElement(char action, struct Node **b_list)
{
    if (action == 'l')
    {
        return next(*b_list);
    }
    else
    {
        return prev(*b_list);
    }
}

int findNextLegalMove(s_game *game, char action, struct Node **b_list, int *currentActiveColumn, int *skippedColumns)
{
    int sCol = game->board.sourceColumn;
    int whiteTurn = isWhiteTurn(*game);

    struct Node *nextElement = getListElement(action, b_list);
    if (nextElement == NULL)
        return 0;

    int step = nextElement->data;
    int move = getNextMoveCalculation(sCol, step, whiteTurn);

    int valid = validateNextMove(game, move, currentActiveColumn, skippedColumns, action, b_list);
    if (valid == 0)
        return 0;

    makeMove(b_list, action);
    *currentActiveColumn = move;

    return 0;
}

int checkGameTurnPawns(s_boardColumn currentColumn, char color)
{
    return (currentColumn.pawnIds.count == 0 || currentColumn.pawnIds.ptr[0].color != color);
}

void printSelectPawn(s_game *game, WINDOW *gameWin)
{
    game->board.isBarActive = 0;
    int srcIds[] = {5};

    hideMenu();
    refresh();
    renderMenu(srcIds, sizeof(srcIds) / sizeof(srcIds[0]), 0, 0, 19, NULL, game);
    renderBoard(game, gameWin, 1, 0, NULL);
}

int checkCourtEnter(s_game *game, int colIdx, int *moveArr, int moves, int whiteTurn)
{
    for (int k = 0; k < moves; ++k)
    {
        int nextColIdx = getNextMoveCalculation(colIdx, moveArr[k], whiteTurn);
        if (nextColIdx == BLACK_COURT || nextColIdx == WHITE_COURT)
        {
            return colIdx;
        }
    }
    return NOT_FOUND;
}

int getCurrentMinDiceVal(s_game game)
{
    int dice1 = game.diceInfo.dice[0];
    int dice2 = game.diceInfo.dice[1];
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
    return minStep;
}

void printfUserData(char player[], char name[], int score, int basePosY)
{
    mvprintw(basePosY, 0, "%s", player);
    mvprintw(basePosY + 1, 0, "- nazwa: %s", name);
    mvprintw(basePosY + 2, 0, "- punkty: %d", score);
}

void showEndGameInfo(s_game game)
{
    int basePosX = 0;
    int basePosY = 2;

    mvprintw(basePosY, 0, "Wyniki gry: ");
    mvprintw(basePosY + 2, 0, "- wygral: %s", game.endGame.winner == 1 ? game.usersData.player1.name : game.usersData.player2.name);

    mvprintw(basePosY + 4, 0, "Aktualne dane: ");

    printfUserData("Gracz 1: ", game.usersData.player1.name, game.usersData.player1.score, 6);

    printfUserData("Gracz 2: ", game.usersData.player2.name, game.usersData.player2.score, 10);
}