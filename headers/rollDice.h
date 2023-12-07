#include "../headers/globalStructs.h"

#ifndef ROLLDICE_H
#define ROLLDICE_H

// int *rollDice(int *diceSize);
void rollDice(int *diceSize, s_game *game);
int randomNum(int min, int max);
#endif // ROLLDICE_H
