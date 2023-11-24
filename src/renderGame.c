
#include "ncurses.h"
#include "stdlib.h"
#include "../headers/menu.h"
#include "../headers/vectorPawn.h"
#include "../headers/playerTurn.h"
#include "../headers/globalStructs.h"

// #define COLUMNS_COUNT 24
// #define BOARD_COLUMN_HEIGHT 5
// #define PAWNS_COUNT 15
// #define MAX_PAWN_ON_COL 5
// #define COLUMNGAP 3

// typedef struct
// {
//     vector_t_pawn pawnIds;
//     //    int pawnIds[MAX_PAWN_ON_COL];
//     int colX;
//     int colY;
//     int isReversed;
// } s_boardColumn;

// typedef struct
// {
//     s_pawn pawnOnBar;
//     s_boardColumn columns[COLUMNS_COUNT];
//     s_boardColumn bar;
// } s_board;

// typedef struct
// {
//     char turn;
//     s_board board;
//     s_pawn wPawns[PAWNS_COUNT];
//     s_pawn bPawns[PAWNS_COUNT];
//     s_pawn removePawns[2 * PAWNS_COUNT];
//     int rollDice[2];
// } s_game;

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
    for (int i = 0; i < COLUMNS_COUNT; ++i)
    {
        game->turn = 'w';
        game->initialDiceValueW = -1;
        game->initialDiceValueB = -1;

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
    char *pawnLabel = color == 'b' ? "\u2592\u2592" : "\u2593\u2593";
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

// render columns

void findColumnWithPawn(s_game game, int *currentActiveColumn)
{
    if (game.turn == 'w')
    {
        for (int i = 0; i < COLUMNS_COUNT; ++i)
        {
            // vector_t_pawn pawn = game.board.columns[i].pawnIds;
            s_boardColumn boardColumn = game.board.columns[i];
            vector_t_pawn currentColumn = boardColumn.pawnIds;

            if ((currentColumn.count > 0))
            {
                char color = currentColumn.ptr[0].color;
                if (color == 'w')
                {
                    *currentActiveColumn = i;
                    break;
                }
            }
        }
    }
}

void renderBoard(s_game game, WINDOW *gameWin, int selectColumn)
{
    // selectColumn - 1 to active
    int currentActiveColumn = -1;

    do
    {
        if (selectColumn && currentActiveColumn == -1)
        {
            findColumnWithPawn(game, &currentActiveColumn);
        }
        for (int i = 0; i < COLUMNS_COUNT; ++i)
        {
            s_boardColumn boardColumn = game.board.columns[i];
            int additionalOffsetLeft = 0;
            if (boardColumn.colX < 6 || boardColumn.colX > 17)
            {
                additionalOffsetLeft = 4;
            }
            if (i == currentActiveColumn)
            {
                attron(COLOR_PAIR(2));
                renderColumn(boardColumn, game, gameWin, additionalOffsetLeft);

                attroff(COLOR_PAIR(2));
            }
            else
            {
                renderColumn(boardColumn, game, gameWin, additionalOffsetLeft);
            }
        }
        // render bar that is between 6 and 7 column
        int barElements = (MAX_PAWN_ON_COL * 2) + COLUMNGAP;
        int offset_y = 3;
        for (int i = 1; i < barElements; ++i)
        {
            mvprintw(i + offset_y, 1 + (6 * 4), "||");
        }
        wrefresh(gameWin);

        if (selectColumn == 1)
        {
            int ch = getch();
            curs_set(0);
            if (ch == KEY_LEFT)
            {
                if (currentActiveColumn > 0)
                {
                    currentActiveColumn--;
                }
            }
            else if (ch == KEY_RIGHT)
            {
                if (currentActiveColumn < COLUMNS_COUNT - 1)
                {
                    currentActiveColumn++;
                }
            }
            else if (ch == KEY_DOWN)
            {
                if (currentActiveColumn < COLUMNS_COUNT / 2)
                {
                    currentActiveColumn += COLUMNS_COUNT / 2;
                }
            }
            else if (ch == KEY_UP)
            {
                if (currentActiveColumn > ((COLUMNS_COUNT / 2) - 1))
                {
                    currentActiveColumn -= COLUMNS_COUNT / 2;
                }
            }
        }

    } while (selectColumn);
}

void renderPossibleDiceMoves(int *rollDice, int size)
{
    for (int i = 0; i < size; ++i)
    {
        mvprintw(10, 29 + (i * 2), "%d", rollDice[i]);
    }
}

void renderGame()
{
    erase();
    refresh();

    mvprintw(0, 0, "Backgammon (press q to exit)");
    s_game game = {};
    initializeGame(&game);

    // showPlayerInfo(game);

    int ySize = 14, xSize = 52, yStart = 3, xStart = 0;
    WINDOW *gameWin = newwin(ySize, xSize, yStart, xStart);
    wrefresh(gameWin);
    box(gameWin, 0, 0);
    wrefresh(gameWin);
    renderBoard(game, gameWin, 0);

    // decide who starts
    // int startRollsIds[] = {7};
    // renderMenu(startRollsIds, sizeof(startRollsIds) / sizeof(startRollsIds[0]), 0, 0, 19, NULL, &game);
    updateInitialDiceValues(&game);
    wrefresh(gameWin);

    // this will render menu with roll dice option
    //

    int menuIds[] = {4};
    int diceSize = 0;
    int *rollDice = (int *)renderMenu(menuIds, 1, 0, 0, 19, &diceSize, &game);

    curs_set(0);
    renderPossibleDiceMoves(rollDice, diceSize);

    // render to select a pawn from column
    int moveIds[] = {5};

    renderMenu(moveIds, sizeof(moveIds) / sizeof(moveIds[0]), 0, 0, 19, NULL, &game);
    renderBoard(game, gameWin, 1);

    free(rollDice);

    wrefresh(gameWin);
}