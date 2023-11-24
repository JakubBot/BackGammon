//
// Created by jakub bot on 18/11/2023.
//

#ifndef ROLLDICE_H
#define ROLLDICE_H

typedef struct
{
  int size;
  int *dice;
} s_dice;

int *rollDice(int *diceSize);
int randomNum(int min, int max);
#endif // ROLLDICE_H
