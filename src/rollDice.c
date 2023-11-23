#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../headers/rollDice.h"

int randomNum(int min, int max)
{
    return rand() % max + min;
}

int *rollDice(int *diceSize)
{
    int tempArr[2] = {0, 0};

    for (int i = 0; i < 2; i++)
    {
        tempArr[i] = randomNum(1, 6);
    }

    if (tempArr[0] == tempArr[1])
        *diceSize = 4;
    else
        *diceSize = 2;

    int *dice = (int *)malloc(*diceSize * sizeof(int));
    srand(time(NULL));
    // int *dice = (int *)malloc(size * sizeof(int));
    if (dice == NULL)
    {
        for (int i = 0; i < *diceSize; i++)
        {
            dice[i] = -1;
        }
        return dice;
    }

    for (int i = 0; i < *diceSize; i++)
    {
        if (*diceSize == 4)
        {
            dice[i] = tempArr[0];
        }
        else
        {
            dice[i] = tempArr[i];
        }
    }

    return dice;
}
