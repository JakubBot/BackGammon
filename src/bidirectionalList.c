#include <stdio.h>
#include <stdlib.h>
#include "../headers/bidirectionalList.h"
#include "ncurses.h"

struct Node* createNode(int value) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Błąd alokacji pamięci.\n");
        exit(1);
    }
    newNode->data = value;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

void append(struct Node** head, int value) {
    struct Node* newNode = createNode(value);
    if (*head == NULL) {
        *head = newNode;
    } else {
        struct Node* current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
        newNode->prev = current;
    }
}

struct Node* next(struct Node* current) {
    return current->next;
}

int moveNext(struct Node** current) {
    if (*current != NULL && next(*current) != NULL) {
        *current = (*current)->next;
        return 1;
    }
    return 0;
}

struct Node* prev(struct Node* current) {
    return current->prev;
}


int movePrev(struct Node** current) {
    if (*current != NULL && prev(*current) != NULL) {
        *current = (*current)->prev;
        return 1;
    }
    return 0;
}

void moveToTop(struct Node** current) {
    while (*current != NULL && (*current)->prev != NULL) {
        *current = (*current)->prev;
    }
}


int countDepthRecursive(struct Node* current) {
    int steps = 0;
    while (current->prev != NULL) {
        steps++;
        current = current->prev;
    }
    return steps;
}

void removeFirstNodeWithValue(struct Node** head, int value) {
    struct Node* current = *head;

    while (current != NULL) {
        if (current->data == value) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            } else {
                *head = current->next;
            }

            if (current->next != NULL) {
                current->next->prev = current->prev;
            }

            free(current);
            return;
        }

        current = current->next;
    }
}


void removeLastNode(struct Node** head) {
    if (*head == NULL) {
        return;
    }

    struct Node* current = *head;
    struct Node* previous = NULL;

    while (current->next != NULL) {
        previous = current;
        current = current->next;
    }

    if (previous != NULL) {
        previous->next = NULL;
    } else {
        *head = NULL;
    }

    free(current);
}

void freeList(struct Node** b_list) {
    while (*b_list != NULL) {
        struct Node* temp = *b_list;
        *b_list = (*b_list)->next;
        free(temp);
    }
}