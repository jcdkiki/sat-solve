#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int n_defined = 0;

task_t task;
ssize_t *score;

void print_vars()
{
    for (int i = 0; i < task.n_vars; i++) {
        int v = (int)task.vars[i];
        printf("%d ", v == VAR_UNDEFINED ? 0 : v);
    }
    printf("\n");
}

int is_ok()
{
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

        if (!ok) {
            return 0;
        }
    }

    return 1;
}

int full_zero()
{
    for (int i = 0; i < task.n_clauses; i++) {
        clause_t *c = &task.clauses[i];
        int has_undefined = 0;
        int has_one = 0;

        for (int j = 0; j < c->size; j++) {
            if (c->xs[j] > 0 && task.vars[c->xs[j] - 1] == 1) {
                has_one = 1;
            }
            if (c->xs[j] < 0 && task.vars[-c->xs[j] - 1] == 0) {
                has_one = 1;
            }
            if (c->xs[j] > 0 && task.vars[c->xs[j] - 1] == VAR_UNDEFINED) {
                has_undefined = 1;
            }
            if (c->xs[j] < 0 && task.vars[-c->xs[j] - 1] == VAR_UNDEFINED) {
                has_undefined = 1;
            }
        }

        if (!has_undefined && !has_one) {
            return 1;
        }
    }

    return 0;
}

int n_rej = 0;
size_t n_propagations = 0;
size_t n_solutions_skipped = 0;

int unit_propagation()
{
    int ok = 0;

    for (int j = 0; j < task.n_clauses; j++) {
        clause_t *c = &task.clauses[j];
        int n_undefined = 0;
        int undefined_i;
        int undefined_val;
        int clause_ok = 0;

        for (int k = 0; k < c->size; k++) {
            if (task.vars[abs(c->xs[k]) - 1] == VAR_UNDEFINED) {
                n_undefined++; 
                undefined_i = abs(c->xs[k]) - 1;
                undefined_val = (c->xs[k] > 0);
            }

            if (c->xs[k] > 0 && task.vars[c->xs[k] - 1] == 1) {
                clause_ok = 1;
            }
            if (c->xs[k] < 0 && task.vars[-c->xs[k] - 1] == 0) {
                clause_ok = 1;
            }
        }

        if (clause_ok) {
            continue;
        }

        if (n_undefined == 1) {
            task.vars[undefined_i] = undefined_val;
            n_defined++;
            return undefined_i;
        }
    }

    return -1;
}

int pick_variable()
{
    int best_i = -1;
    ssize_t best_score = -1;

    for (int i = 0; i < task.n_vars; i++) {
        if (task.vars[i] == VAR_UNDEFINED && score[i] > best_score) {
            best_i = i;
            best_score = score[i];
        }
    }

    return best_i;
}

int cur_var_i;

void reject()
{
    n_rej++;
    n_solutions_skipped += (1 << (task.n_vars - n_defined));    
    score[cur_var_i]++;
}

void check_sat()
{
    int propogate_i = unit_propagation();
    if (propogate_i != -1) {
        n_propagations++;
        n_solutions_skipped += (1 << (task.n_vars - n_defined));

        check_sat();
        
        n_defined--;
        task.vars[propogate_i] = VAR_UNDEFINED;
        return;
    }

    if (full_zero())
    {
        reject();
        return;
    }

    if (is_ok()) {
        print_vars();
        free_task(&task);

        fprintf(stderr, "rejected: %d\n", n_rej);
        fprintf(stderr, "solutions skipped: %ld\n", n_solutions_skipped);
        fprintf(stderr, "propogations: %ld\n", n_propagations);

        fprintf(stderr, "scores:\n");
        for (int i = 0; i < task.n_vars; i++) {
            fprintf(stderr, "  x%d: %ld\n", i + 1, score[i]);
        }

        exit(0);
    }

    if (n_defined == task.n_vars) {
        return;
    }

    n_defined++;
    
    int prev_var_i = cur_var_i;
    cur_var_i = pick_variable();

    task.vars[cur_var_i] = 0;
    check_sat();
    
    task.vars[cur_var_i] = 1;
    check_sat();

    n_defined--;
    task.vars[cur_var_i] = VAR_UNDEFINED;
    cur_var_i = prev_var_i;
}

int main()
{
    read_task(&task, stdin);

    score = malloc(sizeof(size_t) * task.n_vars);
    memset(score, 0, sizeof(size_t) * task.n_vars);

    check_sat();

    printf("not satisfiable\n");
    free_task(&task);
    return 1;
}