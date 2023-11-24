#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include "headers/menu.h"

#define COLOR_COLUMN 20

int main()
{
    setlocale(LC_ALL, "");
    srand(time(NULL));

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_color(COLOR_COLUMN,936,277,434);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_COLUMN, COLOR_BLACK);
    refresh();

    int menuIds[] = {0, 1, 2, 3};
    renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), 0, 0, 0,NULL,NULL);

    getch();
    endwin();

    return 0;
}