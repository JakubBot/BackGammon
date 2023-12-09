#include <stdio.h>
#include <stdlib.h>
#include "../headers/bidirectionalList.h"
#include "ncurses.h"

struct Node *createNode(int value)
{
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (newNode == NULL)
    {
        printf("Błąd alokacji pamięci.\n");
        exit(1);
    }
    newNode->data = value;
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

void append(struct Node **head, int value)
{
    struct Node *newNode = createNode(value);
    if (*head == NULL)
    {
        *head = newNode;
    }
    else
    {
        struct Node *current = *head;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = newNode;
        newNode->prev = current;
    }
}

struct Node *next(struct Node *current)
{

    if (current != NULL && current->next != NULL)
    {
        return current->next;
    }
    return NULL;

    // return current->next;
}

struct Node *prev(struct Node *current)
{

    if (current != NULL && current->prev != NULL)
    {
        return current->prev;
    }

    return NULL;
}

int moveNext(struct Node **current)
{
    if (*current != NULL && next(*current) != NULL)
    {
        *current = (*current)->next;
        return 1;
    }
    return 0;
}

int movePrev(struct Node **current)
{
    if (*current != NULL && prev(*current) != NULL)
    {
        *current = (*current)->prev;
        return 1;
    }
    return 0;
}

void moveToTop(struct Node **current)
{
    while (*current != NULL && (*current)->prev != NULL)
    {
        *current = (*current)->prev;
    }
}

int countDepthRecursive(struct Node *current)
{
    int steps = 0;
    while (current->prev != NULL)
    {
        steps++;
        current = current->prev;
    }
    return steps;
}

void removeFirstNodeWithValue(struct Node **head, int value)
{
    struct Node *current = *head;

    while (current != NULL)
    {
        if (current->data == value)
        {
            if (current->prev != NULL)
            {
                current->prev->next = current->next;
            }
            else
            {
                *head = current->next;
            }

            if (current->next != NULL)
            {
                current->next->prev = current->prev;
            }
            if (current != NULL)
            {
                free(current);
            }
            return;
        }
        if (next(current) != NULL)
        {
            current = current->next;
        }
        else
        {
            break;
        }
    }
}

struct Node *removeLastNode(struct Node *head)
{
    if (head == NULL)
        return NULL;

    if (head->next == NULL)
    {
        free(head);
        return NULL;
    }

    struct Node *second_last = head;
    while (second_last->next->next != NULL)
        second_last = second_last->next;

    free(second_last->next);

    second_last->next = NULL;

    return head;
}

void freeList(struct Node **b_list)
{
    while (*b_list != NULL)
    {

        struct Node *temp = *b_list;
        *b_list = (*b_list)->next;
        free(temp);
    }
}

int findLevel(struct Node *head, int value)
{
    int level = 0;
    struct Node *current = head;

    while (current != NULL)
    {
        if (current->data == value)
        {
            return level;
        }
        current = current->next;
        level++;
    }

    // Wartość nie została znaleziona
    return -1;
}