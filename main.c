#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include "headers/menu.h"

int main()
{
    setlocale(LC_ALL, "");
    srand(time(NULL));

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    refresh();

    int menuIds[] = {0, 1, 2, 3};
    renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), 0, 0, 0,NULL);

    getch();
    endwin();

    return 0;
}