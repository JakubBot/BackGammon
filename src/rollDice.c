#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// #include "../headers/rollDice.h"
#include "../headers/utils.h"
#include "../headers/globalStructs.h"

int randomNum(int min, int max)
{
    return rand() % max + min;
}

void rollDice(int *diceSize, s_game *game)
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

    // int *dice = (int *)malloc(*diceSize * sizeof(int));

    // if (dice == NULL)
    // {
    //     printf("Błąd alokacji pamięci.\n");
    //     exit(1);
    // }

    // if (dice == NULL)
    // {
    //     for (int i = 0; i < *diceSize; i++)
    //     {
    //         dice[i] = -1;
    //     }
    //     return dice;
    // }
    if (*diceSize == 4)
    {
        for (int i = 0; i < *diceSize; i++)
        {
            game->diceInfo.dice[i] = tempArr[0];
        }
    }
    else
    {
        game->diceInfo.dice[0] = min(tempArr[0], tempArr[1]);
        game->diceInfo.dice[1] = max(tempArr[0], tempArr[1]);
        game->diceInfo.dice[2] = 0;
        game->diceInfo.dice[3] = 0;
    }

    for (int i = 0; i < 2; ++i)
    {
        game->diceInfo.initialDiceValues[i] = game->diceInfo.dice[i];
    }
    // game->diceInfo.dice = diceRes;

    // return dice;
}
