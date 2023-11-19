//
// Created by jakub bot on 18/11/2023.
//

#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    int id;
    char color;
} s_pawn;

typedef struct {
    int allocated_size;
    int count;
    s_pawn *ptr;
} vector_t;

void init(vector_t *v);

void reallocate(vector_t *v, int reallocate_size);

void push_back(vector_t *v, s_pawn val);

s_pawn pop_back(vector_t *v);

#endif //VECTOR_H
