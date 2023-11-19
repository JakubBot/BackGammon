#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int randomNum(int min, int max) {
    return rand() % max + min;
}

int* rollDice(int size) {
    srand(time(NULL));
    int* dice = (int*)malloc(size * sizeof(int));
    if (dice == NULL) {
        for (int i =0; i < size; i++) {
            dice[i] = -1;
        }
      return dice;
    }
    // Losowanie dwóch wartości od 1 do 6
    for (int i =0; i < size; i++) {
        dice[i] = randomNum(1,6);
    }
    return dice;
}
