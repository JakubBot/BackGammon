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
} vector_t_pawn;

void init(vector_t_pawn *v);

void reallocate(vector_t_pawn *v, int reallocate_size);

void push_back(vector_t_pawn *v, s_pawn val);

s_pawn pop_back(vector_t_pawn *v);

#endif //VECTOR_H
