#include "ncurses.h"
#include "../headers/globalStructs.h"
#include "../headers/vectorPawn.h"
#include "../headers/utils.h"

void movePawn(s_game *game) {

    int sourceCol = game->board.sourceColumn;
    int targetCol = game->board.targetColumn;

    vector_t_pawn* sourcePawnIds = findColumnPawnIds(game,sourceCol);
    vector_t_pawn* targetPawnIds = findColumnPawnIds(game,targetCol);

    s_pawn pawn = pop_back(sourcePawnIds);
//    s_pawn pawn = pop_back(&game->board.columns[sourceCol].pawnIds);
    push_back(targetPawnIds,pawn);
//    push_back(&game->board.columns[targetCol].pawnIds, pawn);

    game->board.sourceColumn = -1;
    game->board.targetColumn = -1;

    refresh();
}