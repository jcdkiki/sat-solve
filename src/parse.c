#include "parse.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define INITIAL_CLAUSE_CAPACITY 8

void read_task(task_t *task, FILE *file)
{
    int cur_clause = 0;

    while (1) {
        size_t n;
        char *start = NULL;
        
        ssize_t ret = getline(&start, &n, file);
        char *line = start;
        if (ret == -1) {
            return;
        }

        while (isspace(*line)) {
            line++;
        }

        switch (*line) {
            case 'c':
                continue;
            case 'p':
            {
                line++;
                sscanf(line, "%*s %d %d", &task->n_vars, &task->n_clauses);
                
                task->clauses = malloc(sizeof(clause_t) * task->n_clauses);
                task->vars = malloc(sizeof(unsigned char) * task->n_vars);
                memset(task->vars, VAR_UNDEFINED, sizeof(unsigned char) * task->n_vars);

                for (int i = 0; i < task->n_clauses; i++) {
                    task->clauses[i].capacity = INITIAL_CLAUSE_CAPACITY;
                    task->clauses[i].size = 0;
                    task->clauses[i].xs = malloc(sizeof(int) * INITIAL_CLAUSE_CAPACITY);
                }
                break;
            }
            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9': case '-':
            {
                int val, n_read;
                while (sscanf(line, "%d%n", &val, &n_read) == 1) {
                    line += n_read;
                    if (val == 0) {
                        continue;
                    }

                    clause_t *c = &task->clauses[cur_clause];
                    
                    if (c->size == c->capacity) {
                        c->capacity *= 2;
                        c->xs = realloc(c->xs, sizeof(int) * c->capacity);
                    }

                    c->xs[c->size++] = val;
                }
                cur_clause++;
                break;
            }
            default:
                break;
        }

        free(start);
    }
}

void free_task(task_t *task)
{
    for (int i = 0; i < task->n_clauses; i++) {
        free(task->clauses[i].xs);
    }

    free(task->clauses);
    free(task->vars);
}