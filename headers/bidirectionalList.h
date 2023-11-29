#ifndef BIDIRECTIONALLIST_H
#define BIDIRECTIONALLIST_H

struct Node
{
    int data;
    struct Node *prev; // Wskaźnik na poprzedni element
    struct Node *next; // Wskaźnik na następny element
};

struct Node *createNode(int value);
void append(struct Node **head, int value);

struct Node *next(struct Node *current);
struct Node *prev(struct Node *current);
int moveNext(struct Node** current);
int movePrev(struct Node** current);
int countDepthRecursive(struct Node* node);
void removeFirstNodeWithValue(struct Node** head, int value);
void removeLastNode(struct Node** head);
void moveToTop(struct Node** current);
#endif // BIDIRECTIONALLIST_H
