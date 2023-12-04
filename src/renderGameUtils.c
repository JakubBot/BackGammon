//
// Created by jakub bot on 30/11/2023.
//

// #include "../headers/renderGameUtils.h"

#include "../headers/globalStructs.h"
#include "../headers/utils.h"
#include "../headers/bidirectionalList.h"
#include "../headers/fileActions.h"

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

// int isAnyPawnOnBar(s_game game)
// {
// }

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
    int *dice = game->diceInfo.dice;
    // initial value
    append(b_list, 0);
    if (game->diceInfo.isDoublet)
    {
        for (int i = 0; i < game->diceInfo.diceSize; ++i)
        {
            append(b_list, dice[i] * (i + 1));
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
    push_back(boardColumn, (s_pawn){.id = 1, .color = c});
}

void getInitialColumnsIds(vector_t_pawn *boardColumn, int col)
{
    char c = col == 0 || col == 11 || col == 16 || col == 18 ? 'b' : 'w';
    if (col == 11 || 23 == col)
    {
        addPawns(boardColumn, 0, c);
        addPawns(boardColumn, 1, c);
    }
    else if (col == 0 || col == 12)
    {
        addPawns(boardColumn, 2, c);
        addPawns(boardColumn, 3, c);
        addPawns(boardColumn, 4, c);
        addPawns(boardColumn, 5, c);
        addPawns(boardColumn, 6, c);
    }
    else if (col == 4 || col == 16)
    {
        addPawns(boardColumn, 12, c);
        addPawns(boardColumn, 13, c);
        addPawns(boardColumn, 14, c);
    }
    else if (col == 6 || col == 18)
    {
        addPawns(boardColumn, 7, c);
        addPawns(boardColumn, 8, c);
        addPawns(boardColumn, 9, c);
        addPawns(boardColumn, 10, c);
        addPawns(boardColumn, 11, c);
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

        moveToTop(&b_list);
        for (int i = 0; i < moveIndex; ++i)
        {
            removeLastNode(&b_list);
        }
    }
}

void updateDice(s_game *game, struct Node *b_list)
{
    int moveIndex = getMoveDicePosition(b_list, *game);

    if (moveIndex != 0)
    {

        game->diceInfo.availableDiceMoves = game->diceInfo.availableDiceMoves - moveIndex;

        int dice1 = game->diceInfo.dice[0];
        int dice2 = game->diceInfo.dice[1];

        if (dice1 != -1 && dice2 != -1 && moveIndex != 3)
        {
            removeLastNode(&b_list);
        }

        int diceVal = game->diceInfo.dice[moveIndex - 1];
        game->diceInfo.dice[moveIndex - 1] = -1;

        removeFirstNodeWithValue(&b_list, diceVal);
    }
}