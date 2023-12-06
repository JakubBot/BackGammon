#include "ncurses.h"
#include "../headers/globalStructs.h"
#include "../headers/vectorPawn.h"
#include "../headers/utils.h"

void capturePawn(s_game *game, int colX)
{
    int sourceColId = game->board.sourceColumn;
    int targetColId = game->board.targetColumn;

    vector_t_pawn *targetPawnIds = findColumnPawnIds(game, targetColId);
    if ((targetPawnIds->count == 1) && (targetPawnIds->ptr[0].color != game->turn))
    {

        vector_t_pawn *sourcePawnIds = findColumnPawnIds(game, colX);
        vector_t_pawn *barPawnIds = &game->board.bar.pawnIds;

        s_pawn pawn = pop_back(sourcePawnIds);

        push_back(barPawnIds, pawn);
    }
}

void resetGameState(s_game *game)
{
    game->board.sourceColumn = -1;
    game->board.targetColumn = -1;
    game->board.isBarActive = 0;
    game->board.pawnMoveToCourt = 0;
    game->board.forcedTargetColumn = -10;
    game->removeFurthestPawn = 0;
}

void handleMove(s_game *game, vector_t_pawn *sourcePawnIds, vector_t_pawn *targetPawnIds, vector_t_pawn *courtPawns, vector_t_pawn *barPawnIds, int isBarActive)
{
    if (game->board.pawnMoveToCourt)
    {
        s_pawn pawn = pop_back(sourcePawnIds);
        push_back(courtPawns, pawn);
    }
    else if (isBarActive)
    {
        int pawnIndex = findPawnIndex(*barPawnIds, game->turn);

        if (pawnIndex != NOT_FOUND)
        {
            s_pawn pawn = erasePawn(barPawnIds, pawnIndex);
            push_back(targetPawnIds, pawn);
        }
    }
    else
    {
        s_pawn pawn = pop_back(sourcePawnIds);
        push_back(targetPawnIds, pawn);
    }
}

void movePawn(s_game *game)
{
    int sColId = game->board.sourceColumn;
    int tColId = game->board.targetColumn;

    capturePawn(game, tColId);

    vector_t_pawn *sourcePawnIds = findColumnPawnIds(game, sColId);
    vector_t_pawn *targetPawnIds = findColumnPawnIds(game, tColId);
    vector_t_pawn *courtPawns = &game->courtPawns;

    vector_t_pawn *barPawnIds = &game->board.bar.pawnIds;

    int isBarActive = 0;
    for (int i = 0; i < barPawnIds->count; i++)
    {
        if (barPawnIds->ptr[i].color == game->turn)
        {
            isBarActive = 1;
        }
    }

    handleMove(game, sourcePawnIds, targetPawnIds, courtPawns, barPawnIds, isBarActive);

    resetGameState(game);
    refresh();
}