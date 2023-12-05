//
// Created by jakub bot on 30/11/2023.
//
#include <stdlib.h>
#include "../headers/globalStructs.h"
#include "../headers/utils.h"
#include "../headers/menuUtils.h"

int getTextLength(char *text)
{
    int i = 0;
    while (text[i] != '\0')
    {
        i++;
    }
    return i;
}

s_btnOption findBtn(s_btnOption *btns, int id, int btnCount)
{
    for (int i = 0; i < btnCount; i++)
    {
        if (btns[i].id == id)
        {
            return btns[i];
        }
    }
    return btns[0];
}

char *formatOptionText(s_game *game)
{
    if (game == NULL)
        return "";
    // Przykładowa logika zależna od zmiennej startingDice
    if (game->turn == 'w')
    {
        return "Player 1 rolls";
    }
    else
    {
        return "Player 2 rolls";
    }
}

char *formatGameWinnerText(s_game *game)
{
    if (game == NULL)
        return "";

    if (game->endGame.winner == 1)
    {
        return "Player 1 wins";
    }
    else if (game->endGame.winner == 2)
    {
        return "Player 2 wins";
    }
    return "";
}

s_btnOption *getBtnElements(int btnIds[], int btnCount, s_game *game)
{
    s_btnOption data[] = {
        (s_btnOption){"Start", 0, 's'},
        (s_btnOption){"Load Game", 1, 'l'},
        (s_btnOption){"Author", 2, 'a'},
        (s_btnOption){"Exit", 3, 'e'},
        (s_btnOption){"Roll Dice", 4, 'r'},
        (s_btnOption){"Select a pawn you want to move", 5, 'm'},
        (s_btnOption){"Select a column", 6, 'c'},
        (s_btnOption){formatOptionText(game), 7, 'd'},
        (s_btnOption){"Exit & Save changes", 8, 'h'},
        (s_btnOption){"You do not have any moves", 9, 'v'},
        (s_btnOption){formatGameWinnerText(game), 10, 'g'},
    };
    int count = sizeof(data) / sizeof(data[0]);

    s_btnOption *btns = malloc(btnCount * sizeof(s_btnOption));

    if (btns != NULL)
    {
        for (int i = 0; i < btnCount; ++i)
        {
            btns[i] = findBtn(data, btnIds[i], count);
        }
    }
    return btns;
}