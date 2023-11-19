#include <stdio.h>
#include <ncurses.h>
#include "headers/menu.h"


int main() {


    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    start_color();

    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    refresh();

    int menuIds[] = { 0,1,2,3};
    renderMenu(menuIds,sizeof(menuIds) / sizeof(menuIds[0]),0,0,0);


    getch();
    endwin();

    return 0;
}