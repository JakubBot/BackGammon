//
// Created by jakub bot on 30/11/2023.
//

#ifndef BACKGAMMON2_MENUUTILS_H
#define BACKGAMMON2_MENUUTILS_H

typedef struct btnOption
{
    char *text;
    // char text[50];
    int id;
    char action;
} s_btnOption;

int getTextLength(char *text);
s_btnOption *getBtnElements(int btnIds[], int btnCount, s_game *game);
s_btnOption findBtn(s_btnOption *btns, int id, int btnCount);
char *formatOptionText(s_game *game);

#endif //BACKGAMMON2_MENUUTILS_H
