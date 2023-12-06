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

void menuAction(s_game *game, int *diceSize, int btnIds[], int btnCount, int selectedBtn, int menuPosY, s_btnOption *buttons);
void displayMenuBtn(int btnCount, int menuPosX, int menuPosY, int selectedBtn, s_btnOption *buttons);

void *renderMenu(int btnIds[], int btnCount, int activeBtnId, int menuPosX, int menuPosY, int *diceSize, s_game *game)
{
    curs_set(0);
    s_btnOption *buttons = getBtnElements(btnIds, btnCount, game);
    int selectedBtn = activeBtnId ? activeBtnId : btnIds[0];
    //    int selectedBtn = activeBtnId;
    int currentIndex = 0;

    while (selectedBtn != -1)
    {
        for (int i = 0; i < btnCount; ++i)
        {
            if (btnIds[i] == selectedBtn)
            {
                currentIndex = i;
            }
        }
        displayMenuBtn(btnCount, menuPosX, menuPosY, selectedBtn, buttons);

        int ch = getch();
        if (ch == KEY_LEFT)
        {
            if ((currentIndex - 1) >= 0)
            {
                s_btnOption btn = findBtn(buttons, btnIds[currentIndex - 1], btnCount);
                selectedBtn = btn.id;
                //                selectedBtn--;
            }
            else
            {
                s_btnOption btn = findBtn(buttons, btnIds[btnCount - 1], btnCount);
                selectedBtn = btn.id;
            }
        }
        else if (ch == KEY_RIGHT)
        {
            if ((currentIndex + 1) < btnCount)
            //            if (selectedBtn < btnCount - 1)
            {
                s_btnOption btn = findBtn(buttons, btnIds[currentIndex + 1], btnCount);
                selectedBtn = btn.id;
                //                selectedBtn++;
            }
            else
            {
                s_btnOption btn = findBtn(buttons, btnIds[0], btnCount);
                selectedBtn = btn.id;
            }
        }
        else if (ch == 10)
        {
            break;
        }
        else if (ch == 'q')
        {
            selectedBtn = -1;
            break;
        }
    }
    menuAction(game, diceSize, btnIds, btnCount, selectedBtn, menuPosY, buttons);

    free(buttons);
    return NULL;
}

void displayMenuBtn(int btnCount, int menuPosX, int menuPosY, int selectedBtn, s_btnOption *buttons)
{
    int previusBtnLength = 0;
    for (int i = 0; i < btnCount; i++)
    {
        int txtLen = getTextLength(buttons[i].text);
        WINDOW *menuwin = newwin(3, txtLen + 2, menuPosY, (previusBtnLength + (i * 1) + menuPosX));
        previusBtnLength += txtLen + 2;
        wborder(menuwin, '|', '|', '-', '-', '+', '+', '+', '+');
        if (selectedBtn == buttons[i].id)
        //        if (selectedBtn == i)
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

void menuAction(s_game *game, int *diceSize, int btnIds[], int btnCount, int selectedBtn, int menuPosY, s_btnOption *buttons)
{
    if (selectedBtn != -1)
    {
        s_btnOption btn = findBtn(buttons, selectedBtn, btnCount);
        curs_set(1);

        refresh();

        switch (btn.action)
        {
        case 'd':
            game->turn = (game->turn == 'w') ? 'b' : 'w';
            clrButtonPrints(menuPosY, 3);
            refresh();
            break;
        case 's':
            // start
            renderGame(0);
            break;
        case 'a':
            // author
            authorInfo();

            // set author btn active
            s_btnOption btn = findBtn(buttons, 2, btnCount);
            renderMenu(btnIds, btnCount, btn.id, 0, 0, diceSize, game);
            break;
        case 'r':
            // roll dice

            menuRollDiceAction(game, diceSize);
            break;
        case 'h':
            clear();
            refresh();
            clearGameState(game);
            initGame();
            // exit & save changes
            break;
        case 'l':
            renderGame(1);
            //                fclose(game->file);
            break;
        case 'c':
        case 'm':
            curs_set(0);
            // clrButtonPrints(menuPosY, 3);
            break;
        case 'g':
            clear();
            refresh();
            clearGameState(game);
            break;
        case 'e':
            exit(1);
            break;
            // v - not move
        case 'v':

        default:
            break;
        }
    }
}
