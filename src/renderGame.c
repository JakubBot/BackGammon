
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
    mvprintw(offset_y - 2, offset_x + (x * COLUMN_WIDTH), "%d", labelRowId);
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
    mvprintw(BOARD_COLUMN_HEIGHT + offset_y + columnVerticalOffset + COLUMNGAP + 1, offset_x + (x * COLUMN_WIDTH), "%d", labelRowId);
}

void renderColumn(s_boardColumn boardColumn, int barOffset)
{
    int x = boardColumn.colX, rev = boardColumn.isReversed;
    // reversed 1 means that we want to reverse column
    int offset_y = 4;
    int offset_x = 1 + barOffset;
    int colVerticalOffset = COLUMN_WIDTH;
    int labelRowId = x + 1;
    char *label = (x % 2) == 1 ? rev ? "/\\" : "\\/" : "--";

    vector_t_pawn currCol = boardColumn.pawnIds;
    int colCount = currCol.count;
    char *p_label = currCol.ptr[0].color == 'b' ? BLACK_PAWN : WHITE_PAWN;

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

int getNextMoveCalculation(int firstVal, int secondVal, int whiteTurn, char action)
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

int makeMove(s_game *game, struct Node **b_list, char action)
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

int isValidNextMove(s_game *game, struct Node **b_list, char action, int move)
{

    int pawnGoToHome = allPawnsHome(*game) && ((move == -1) || (move == COLUMNS_COUNT));
    if ((move < 0 || move > (COLUMNS_COUNT - 1)) && !pawnGoToHome && (action == 'l'))
    {
        movePrev(b_list);
        return 0;
    }

    return 1;
}

int findNextLegalMove(s_game *game, char action, struct Node **b_list, int *currentActiveColumn)
{
    int sourceColX = game->board.sourceColumn;
    int whiteTurn = isWhiteTurn(*game);

    int validMv = makeMove(game, b_list, action);
    if (!validMv)
        return 0;

    int step = (*b_list)->data;

    int move = getNextMoveCalculation(sourceColX, step, whiteTurn, action);

    int validNext = isValidNextMove(game, b_list, action, move);
    if (!validNext)
        return 0;

    // dodac zdejmowanie pionkow

    int isValidCol = validPawnToColumnMove(game, move);

    if (!isValidCol)
    {
        int res = findNextLegalMove(game, action, b_list, currentActiveColumn);
        // movePrev(b_list);
        return res;
    }

    *currentActiveColumn = move;

    return 0;
}

void barActiveDots(s_game game)
{
    // additional square for start and end for each color
    if (game.board.sourceColumn == 24 && game.board.isBarActive)
    {
        attron(COLOR_PAIR(2));
        mvprintw(4, 53, SQUARE);
        attroff(COLOR_PAIR(2));
    }
    else
    {
        mvprintw(4, 53, " ");
    }

    if (game.board.sourceColumn == -1 && game.board.isBarActive)
    {
        attron(COLOR_PAIR(2));
        mvprintw(15, 53, SQUARE);
        attroff(COLOR_PAIR(2));
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
        if (currentActiveColumn == -1 || currentActiveColumn == 24)
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

int handleBoardMove(s_game *game, struct Node **b_list, int selectColumn, int targetColumn, int *currentActiveColumn)
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
            findNextLegalMove(game, 'l', b_list, currentActiveColumn);
        }
        else if (ch == KEY_RIGHT)
        {
            findNextLegalMove(game, 'r', b_list, currentActiveColumn);
        }
        else if (ch == 10)
        {
            return 0;
        }
    }
    return 1;
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
        if ((boardColumn.colX == currentActiveColumn) || (boardColumn.colX == game.board.sourceColumn))
        {
            int colorPair = boardColumn.colX == currentActiveColumn ? 2 : 3;
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

void printBar(s_game game, int currentActiveColumn)
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

        if (i == 0 && game.board.isBarActive)
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

    if (targetColumn && *currentActiveColumn == -1)
    {
        *currentActiveColumn = game.board.sourceColumn;
    }
    else if (selectColumn && *currentActiveColumn == -1)
    {
        findColumnWithPawn(game, currentActiveColumn, ' ');
    }
}

void renderBoard(s_game *game, WINDOW *gameWin, int selectColumn, int targetColumn, struct Node *b_list)
{
    // currentActiveColumn represents column(colX) that is active
    int currentActiveColumn = -1;

    do
    {
        setDefaultActiveColumn(*game, &currentActiveColumn, targetColumn, selectColumn);

        barActiveDots(*game);

        printMainBoard(*game, currentActiveColumn);

        // render bar that is between 6 and 7 column
        printBar(*game, currentActiveColumn);

        printBarPawn(*game);

        wrefresh(gameWin);

        int action = handleBoardMove(game, &b_list, selectColumn, targetColumn, &currentActiveColumn);
        if (action == 0)
            break;

    } while (selectColumn || targetColumn);

    setSelectedColumn(game, gameWin, b_list, currentActiveColumn, selectColumn, targetColumn);
}

void setSourceColumn(s_game *game, WINDOW *gameWin)
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
    }
    else
    {
        game->board.isBarActive = 0;
        int srcIds[] = {5, 8};
        clrButtonPrints(19, 3);
        refresh();
        renderMenu(srcIds, sizeof(srcIds) / sizeof(srcIds[0]), 0, 0, 19, NULL, game);
        renderBoard(game, gameWin, 1, 0, NULL);
        clrButtonPrints(19, 3);
    }
}

void moveRepeater(s_game *game, WINDOW *gameWin)
{
    struct Node *b_list = NULL;
    fillBList(&b_list, game);

    while (game->diceInfo.availableDiceMoves > 0)
    {
        // source col
        setSourceColumn(game, gameWin);

        // target col
        int targetIds[] = {6};
        // int targetIds[] = {6, 8};
        renderMenu(targetIds, sizeof(targetIds) / sizeof(targetIds[0]), 0, 0, 19, NULL, game);
        renderBoard(game, gameWin, 0, 1, b_list);

        movePawn(game);

        renderBoard(game, gameWin, 0, 0, NULL);
    }
    freeList(&b_list);
}

void initializeGame(s_game *game)
{
    FILE *file = fopen(CURRENT_GAME, "w");
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

    vector_t_pawn *barPawn = &game->board.bar.pawnIds;

    game->board.isBarActive = 0;

    init(barPawn);

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

    saveCurrentState(game);

    gameLoop(game, gameWin);
}

void renderGame(int loadFromFile)
{

    erase();
    refresh();

    mvprintw(0, 0, "Backgammon");
    s_game game = {};
    initializeGame(&game);

    if (loadFromFile == 1)
    {
        loadFile(&game);
        updateGameFile(1);
    }
    int ySize = 14, xSize = 52, yStart = 3, xStart = 0;
    WINDOW *gameWin = newwin(ySize, xSize, yStart, xStart);
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

void initGame()
{
    int menuIds[] = {0, 1, 2, 3};
    renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), 0, 0, 0, NULL, NULL);
}
