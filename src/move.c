#include "ncurses.h"
#include "../headers/globalStructs.h"
#include "../headers/vectorPawn.h"
#include "../headers/utils.h"

void capturePawn(s_game *game, int colX)
{
    vector_t_pawn *sourcePawnIds = findColumnPawnIds(game, colX);
    vector_t_pawn *barPawnIds = &game->board.bar.pawnIds;

    s_pawn pawn = pop_back(sourcePawnIds);

    push_back(barPawnIds, pawn);

    mvprintw(18, 80, "capture pawn");
}

void movePawn(s_game *game)
{

    int sourceColId = game->board.sourceColumn;
    int targetColId = game->board.targetColumn;

    vector_t_pawn *sourcePawnIds = findColumnPawnIds(game, sourceColId);
    vector_t_pawn *targetPawnIds = findColumnPawnIds(game, targetColId);

    if ((targetPawnIds->count == 1) && (targetPawnIds->ptr[0].color != game->turn))
    {
        capturePawn(game, targetColId);
    }


    vector_t_pawn *barPawnIds = &game->board.bar.pawnIds;

    s_pawn pawn = game->board.isBarActive ? pop_back(barPawnIds) : pop_back(sourcePawnIds);

    push_back(targetPawnIds, pawn);

    game->board.sourceColumn = -1;
    game->board.targetColumn = -1;
    game->board.isBarActive = 0;
    //    game->diceInfo = -1;

    refresh();
}