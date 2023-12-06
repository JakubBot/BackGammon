#include <stdio.h>
#include <stdlib.h>
#include "ncurses.h"
#include "../headers/renderGame.h"
#include "../headers/author.h"
#include "../headers/rollDice.h"
#include "../headers/globalStructs.h"
#include "../headers/playerTurn.h"
#include "../headers/utils.h"
#include "../headers/menuUtils.h"
#include "../headers/renderGameUtils.h"
#include "../headers/bidirectionalList.h"
#include "../headers/fileActions.h"

void menuAction(s_game *game, int *diceSize, int btnIds[], int btnCount, int selectedBtn, int menuY, s_btnOption *buttons);
void displayMenuBtn(int btnCount, int menuX, int menuY, int selectedBtn, s_btnOption *buttons);

int onKeyMove(int *selectedBtn, int index, int btnIds[], int btnCount, s_btnOption *btns)
{
    int ch = getch();

    if (ch == KEY_LEFT)
    {
        if ((index - 1) >= 0)
        {
            s_btnOption btn = findBtn(btns, btnIds[index - 1], btnCount);
            *selectedBtn = btn.id;
        }
        else
        {
            s_btnOption btn = findBtn(btns, btnIds[btnCount - 1], btnCount);
            *selectedBtn = btn.id;
        }
    }
    else if (ch == KEY_RIGHT)
    {
        if ((index + 1) < btnCount)
        {
            s_btnOption btn = findBtn(btns, btnIds[index + 1], btnCount);
            *selectedBtn = btn.id;
        }
        else
        {
            s_btnOption btn = findBtn(btns, btnIds[0], btnCount);
            *selectedBtn = btn.id;
        }
    }
    else if (ch == 10)
    {
        return -1;
    }

    return 0;
}

void assignIndex(int btnIds[], int btnCount, int selectedBtn, int *currentIndex)
{
    for (int i = 0; i < btnCount; ++i)
    {
        if (btnIds[i] == selectedBtn)
        {
            *currentIndex = i;
        }
    }
}

void assignIndexIdToGame(s_game *game, int selectedBtn)
{
    if (game != NULL)
    {
        game->currentMenuBtnIndex = selectedBtn;
    }
}

void *renderMenu(int btnIds[], int btnCount, int activeBtnId, int menuX, int menuY, int *diceSize, s_game *game)
{
    curs_set(0);
    s_btnOption *btns = getBtnElements(btnIds, btnCount, game);
    int selectedBtn = activeBtnId ? activeBtnId : btnIds[0];
    int i = 0;

    while (selectedBtn != -1)
    {

        assignIndex(btnIds, btnCount, selectedBtn, &i);
        displayMenuBtn(btnCount, menuX, menuY, selectedBtn, btns);

        int mv = onKeyMove(&selectedBtn, i, btnIds, btnCount, btns);

        if (mv == -1)
            break;
    }
    assignIndexIdToGame(game, selectedBtn);

    menuAction(game, diceSize, btnIds, btnCount, selectedBtn, menuY, btns);

    free(btns);
    return NULL;
}

void displayMenuBtn(int btnCount, int menuX, int menuY, int selectedBtn, s_btnOption *buttons)
{
    int prevBtnLength = 0;
    for (int i = 0; i < btnCount; i++)
    {
        int txtLen = getTextLength(buttons[i].text);
        WINDOW *menuwin = newwin(3, txtLen + 2, menuY, (prevBtnLength + (i * 1) + menuX));
        prevBtnLength += txtLen + 2;
        wborder(menuwin, '|', '|', '-', '-', '+', '+', '+', '+');
        if (selectedBtn == buttons[i].id)
        {
            wattron(menuwin, COLOR_PAIR(1));
            mvwprintw(menuwin, 1, 1, "%s", buttons[i].text);
            wattron(menuwin, COLOR_PAIR(1));
        }
        else
        {
            mvwprintw(menuwin, 1, 1, "%s", buttons[i].text);
        }

        wrefresh(menuwin);
        delwin(menuwin);
    }
}

void menuRollDiceAction(s_game *game, int *diceSize)
{
    int *diceRes = (int *)rollDice(diceSize);
    game->diceInfo.dice = diceRes;

    for (int i = 0; i < 2; ++i)
    {
        game->diceInfo.initialDiceValues[i] = diceRes[i];
    }

    int isDoublet = game->diceInfo.dice[0] == game->diceInfo.dice[1] ? 1 : 0;
    game->diceInfo.diceSize = *diceSize;
    game->diceInfo.availableDiceMoves = *diceSize + (isDoublet ? 0 : 1);
    game->diceInfo.isDoublet = isDoublet;

    clearSidebarInfo();
    showTurnInfo(*game);
}



void handleBtnAction(char action, s_game *game, int *diceSize, int btnIds[], int btnCount, s_btnOption *btns, int menuY)
{
    switch (action)
    {
    case 's':
    case 'l':
    case 'a':
    case 'e':
        handleInitialBtns(action, game, diceSize, btnIds, btnCount, btns);
        break;
        // game review options
    case 'j':
    case 'n':
    case 'p':
    case 'z':
    case 'x':
    case 'q':
        handleGameReviewOption(action, game);
        break;

    case 'd':
        playerRoll(game, menuY);
        break;

    case 'r':

        menuRollDiceAction(game, diceSize);
        break;
    case 'h':
        exitGame(game);
        // exit & save changes
        break;

    case 'c':
    case 'm':
        curs_set(0);
        break;
    case 'g':
        clear();
        refresh();
        clearGameState(game);
        break;
    }
}

void menuAction(s_game *game, int *diceSize, int btnIds[], int btnCount, int selectedBtn, int menuY, s_btnOption *buttons)
{
    if (selectedBtn != -1)
    {
        s_btnOption btn = findBtn(buttons, selectedBtn, btnCount);
        curs_set(1);

        refresh();

        handleBtnAction(btn.action, game, diceSize, btnIds, btnCount, buttons, menuY);
    }
}
