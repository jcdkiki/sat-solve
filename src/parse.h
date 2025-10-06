#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>

#define VAR_UNDEFINED 255

typedef struct {
    int size;
    int capacity;
    int *xs;
} clause_t;


typedef struct {
    int n_clauses;
    int clauses_capacity;
    clause_t *clauses;
    
    int n_vars;
    unsigned char *vars;
} task_t;

void read_task(task_t *task, FILE *file);
void free_task(task_t *task);

#endif