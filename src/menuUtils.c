#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "../headers/globalStructs.h"
#include "../headers/utils.h"
#include "../headers/menuUtils.h"
#include "../headers/menu.h"
#include "../headers/renderGame.h"
#include "../headers/renderGameUtils.h"
#include "../headers/author.h"
#include "../headers/fileActions.h"

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
    static char text[50];
    if (game == NULL)
        return "";

    if (game->turn == 'w')
    {
        sprintf(text, "%s rolls", game->usersData.player1.name);
        return text;
    }
    else
    {
        sprintf(text, "%s rolls", game->usersData.player2.name);
        return text;
    }
}

char *formatGameWinnerText(s_game *game)
{
    static char text[50];

    if (game == NULL)
        return "";

    if (game->endGame.winner == 1)
    {
        sprintf(text, "%s wins", game->usersData.player1.name);
        return text;
    }
    else if (game->endGame.winner == 2)
    {
        sprintf(text, "%s wins", game->usersData.player2.name);
        return text;
    }
    return "";
}

void btnCont(s_game *game, s_btnOption _data[])
{

    s_btnOption data[] = {
        {"Start", 0, 's'},
        {"Load Game", 1, 'l'},
        {"Author", 2, 'a'},
        {"Exit", 3, 'e'},
        {"Roll Dice", 4, 'r'},
        {"Select a pawn", 5, 'm'},
        {"Select a column", 6, 'c'},
        {formatOptionText(game), 7, 'd'},
        {"Exit & Save changes", 8, 'h'},
        {"You do not have any moves", 9, 'v'},
        {formatGameWinnerText(game), 10, 'g'},
        {"Review your saved game", 11, 'j'},
        {"Next", 12, 'n'},
        {"Prev", 13, 'p'},
        {"Start game", 14, 'z'},
        {"End game", 15, 'x'},
        {"Exit game review", 16, 'q'},
    };
    for (int i = 0; i < MENU_BUTTONS_COUNT; i++)
    {
        _data[i] = data[i];
    }
}

void getBtnElements(int btnIds[], int btnCount, s_game *game, s_btnOption btns[])
{
    s_btnOption data[MENU_BUTTONS_COUNT];
    btnCont(game, data);

    int count = sizeof(data) / sizeof(data[0]);

    // s_btnOption *btns = malloc(btnCount * sizeof(s_btnOption));
    // if (btns == NULL)
    // {
    //     printf("Błąd alokacji pamięci.\n");
    //     exit(1);
    // }
    // if (btns != NULL)
    // {
    for (int i = 0; i < btnCount; ++i)
    {
        btns[i] = findBtn(data, btnIds[i], count);
    }
    // }
    // return btns;
}

void handleGameReviewOption(char action, s_game *game)
{

    switch (action)
    {

    case 'j':
        // read from file
        gameReview();
        break;
    case 'n':
        if (game->gameReview.currentFilePosition < (game->gameReview.fileLength - 1))
        {
            game->gameReview.currentFilePosition++;
        }
        break;
    case 'p':
        if (game->gameReview.currentFilePosition > 0)
        {
            game->gameReview.currentFilePosition--;
        }

        break;
    case 'z':
        game->gameReview.currentFilePosition = 0;
        break;
    case 'x':
        game->gameReview.currentFilePosition = game->gameReview.fileLength - 2;
        break;
    case 'q':
        game->gameReview.exitGameReview = 1;
        break;
    }
}

void handleInitialBtns(char action, s_game *game, int *diceSize, int btnIds[], int btnCount, s_btnOption *buttons)
{

    switch (action)
    {
    case 's':
        // start
        renderGame(0);
        break;
    case 'l':
        renderGame(1);
        break;
    case 'a':
        // author
        authorInfo();

        // set author btn active
        s_btnOption btn = findBtn(buttons, 2, btnCount);
        renderMenu(btnIds, btnCount, btn.id, 0, 0, diceSize, game);
        break;
    case 'e':
        exit(0);
        break;
    }
}

void exitGame(s_game *game)
{
    clear();
    refresh();
    clearGameState(game);
    initGame();
}

void playerRoll(s_game *game, int menuY)
{
    game->turn = (game->turn == 'w') ? 'b' : 'w';
    clrButtonPrints(menuY, 3);
    refresh();
}