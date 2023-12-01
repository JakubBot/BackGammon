#include "../headers/globalStructs.h"
#include "../headers/bidirectionalList.h"

#ifndef MENU_H
#define MENU_H

void *renderMenu(int btnIds[], int btnCount, int activeBtnIn, int menuPosX, int menuPosY, int *diceSize, s_game* game);

#endif // MENU_H
