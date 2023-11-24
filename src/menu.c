#include <stdio.h>
#include <stdlib.h>
#include "ncurses.h"
#include "../headers/renderGame.h"
#include "../headers/author.h"
#include "../headers/rollDice.h"
#include "../headers/globalStructs.h"
#include "../headers/playerTurn.h"

typedef struct btnOption
{
    char *text;
    // char text[50];
    int id;
    char action;
} s_btnOption;

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
        char temp[50] = "Player 1 rolls";
        return "Player 1 rolls";
    }
    else
    {
        char temp[50] = "Player 2 rolls";
        return "Player 2 rolls";
    }
}

s_btnOption *getBtnElements(int btnIds[], int btnCount, s_game *game)
{
    // char *diceRollPlayer = (game.turn == 'w') ? " " : "";
    s_btnOption btnContainer[] = {
        (s_btnOption){"Start", 0, 's'},
        (s_btnOption){"Load Game", 1, 'l'},
        (s_btnOption){"Author", 2, 'a'},
        (s_btnOption){"Exit", 3, 'e'},
        (s_btnOption){"Roll Dice", 4, 'r'},
        (s_btnOption){"Select a pawn you want to move", 5, 'm'},
        (s_btnOption){"Select a column", 6, 'c'},
        (s_btnOption){formatOptionText(game), 7, 'd'},
        // (s_btnOption){"Starting dice roll", 7, 'd'},
    };
    int btnContainerCount = sizeof(btnContainer) / sizeof(btnContainer[0]);

    s_btnOption *btns = malloc(btnCount * sizeof(s_btnOption));

    if (btns != NULL)
    {
        for (int i = 0; i < btnCount; ++i)
        {
            btns[i] = findBtn(btnContainer, btnIds[i], btnContainerCount);
        }
    }
    return btns;
}

void clrButtonPrints(int posY, int nLines)
{
    for (int i = 0; i < nLines; ++i)
    {
        move(posY + i, 0);
        clrtoeol();
    }
}

void *renderMenu(int btnIds[], int btnCount, int activeBtnIn, int menuPosX, int menuPosY, int *diceSize, s_game *game)
{
    curs_set(0);
    s_btnOption *buttons = getBtnElements(btnIds, btnCount, game);
    int selectedBtn = activeBtnIn;
    while (selectedBtn != -1)
    {
        int previusBtnLength = 0;
        for (int i = 0; i < btnCount; i++)
        {
            int txtLen = getTextLength(buttons[i].text);
            WINDOW *menuwin = newwin(3, txtLen + 2, menuPosY, (previusBtnLength + (i * 1) + menuPosX));
            previusBtnLength += txtLen + 2;
            wborder(menuwin, '|', '|', '-', '-', '+', '+', '+', '+');
            if (selectedBtn == i)
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

        int ch = getch();
        if (ch == KEY_LEFT)
        {
            if (selectedBtn > 0)
            {
                selectedBtn--;
            }
        }
        else if (ch == KEY_RIGHT)
        {
            if (selectedBtn < btnCount - 1)
            {
                selectedBtn++;
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
    if (selectedBtn != -1)
    {
        s_btnOption btn = findBtn(buttons, selectedBtn, btnCount);
        curs_set(1);
        switch (btn.action)
        {
        case 'd':
            game->turn = (game->turn == 'w') ? 'b' : 'w';
            clrButtonPrints(menuPosY, 3);
            refresh();
            break;
        case 's':
            // start
            renderGame();
            break;
        case 'a':
            // author
            authorInfo();
            renderMenu(btnIds, btnCount, selectedBtn, 0, 0, diceSize, game);
            break;
        case 'r':
            // roll dice
            return rollDice(diceSize);
            break;
        case 'e':
        case 'm':
        default:
            break;
        }
    }
    free(buttons);
    return NULL;
}
