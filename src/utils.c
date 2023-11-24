#include "ncurses.h"

void clrButtonPrints(int posY, int nLines)
{
    for (int i = 0; i < nLines; ++i)
    {
        move(posY + i, 0);
        clrtoeol();
    }
    refresh();
}
