#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char color;
} s_pawn;

typedef struct {
    int allocated_size;
    int count;
    s_pawn *ptr;
} vector_t_pawn;

void init(vector_t_pawn *v) {
    v->count = 0;
    v->allocated_size = 5;
    v->ptr = (s_pawn *)malloc(v->allocated_size * sizeof(s_pawn));
}

void cleanup(vector_t_pawn *v) {
    free(v->ptr);
    v->ptr = NULL;
    v->count = 0;
    v->allocated_size = 0;
}

void reallocate(vector_t_pawn *v, int reallocate_size) {
    v->allocated_size = reallocate_size;
    s_pawn *newPtr = (s_pawn *)malloc(v->allocated_size * sizeof(s_pawn));
    memcpy(newPtr, v->ptr, v->count * sizeof(s_pawn));
    free(v->ptr);
    v->ptr = newPtr;
}

void push_back(vector_t_pawn *v, s_pawn val) {
    if (v->count == v->allocated_size)
        reallocate(v, 2 * v->allocated_size);
    v->ptr[v->count] = val;
    v->count++;
}

s_pawn pop_back(vector_t_pawn *v) {
    v->count--;
    s_pawn retv = v->ptr[v->count];

    if (4 * v->count <= v->allocated_size)
        reallocate(v, v->allocated_size / 2);
    return retv;
}