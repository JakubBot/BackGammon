#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include "headers/menu.h"
#include "headers/utils.h"
#include "headers/renderGame.h"

// #define COLOR_COLUMN 20
// #define POSSIBLE_COLUMN_MOVE 21

int main()
{
    setlocale(LC_ALL, "");
    srand(time(NULL));

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    init_color(COLOR_COLUMN, 936, 277, 434);
    init_color(POSSIBLE_COLUMN_MOVE, 519, 873, 776);
    init_color(FORCED_COLUMN_MOVE, 200, 396, 541);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_COLUMN_ID, COLOR_COLUMN, COLOR_BLACK);
    init_pair(POSSIBLE_COLUMN_MOVE_ID, POSSIBLE_COLUMN_MOVE, COLOR_BLACK);
    init_pair(FORCED_COLUMN_MOVE_ID, FORCED_COLUMN_MOVE, COLOR_BLACK);
    refresh();
    initGame();
    //    int menuIds[] = {0, 1, 2, 3};
    //    renderMenu(menuIds, sizeof(menuIds) / sizeof(menuIds[0]), 0, 0, 0,NULL,NULL);

    getch();
    endwin();

    return 0;
}