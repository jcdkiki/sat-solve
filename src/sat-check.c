#include <stdio.h>
#include "parse.h"

task_t task;

void check_solution()
{
    int n_read_vars = 0;
    int var;
    while (scanf("%d", &var) == 1) {
        task.vars[n_read_vars] = var;
        n_read_vars++;
    }

    for (int i = 0; i < task.n_clauses; i++) {
        int ok = 0;
        clause_t *c = &task.clauses[i];
        
        for (int j = 0; j < c->size; j++) {
            if (c->xs[j] > 0 && task.vars[c->xs[j] - 1] == 1) {
                ok = 1;
                break;
            }
            if (c->xs[j] < 0 && task.vars[-c->xs[j] - 1] == 0) {
                ok = 1;
                break;
            }
        }

        if (ok == 0) {
            printf("clause #%d failed\n", i);
            return;
        }
    }

    printf("OK!\n");
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s task.cnf\n", argv[0]);
        return 1;
    }

    FILE *fptr = fopen(argv[1], "r");
    if (fptr == NULL) {
        fprintf(stderr, "Failed to open file\n");
        return 1;
    }

    read_task(&task, fptr);
    fclose(fptr);

    check_solution();

    free_task(&task);
    return 0;
}