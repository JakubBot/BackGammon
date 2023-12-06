#include "../headers/globalStructs.h"

#ifndef FILEACTIONS_H
#define FILEACTIONS_H
void saveCurrentState(s_game game);
void updateGameFile(int copyToCurrentFile, s_game game);
void loadFile(s_game *game, int loadPosition);
int countLines(FILE *file);


#endif // FILEACTIONS_H
