#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *name;
    int iters;
} tester_args;

void tester(void *arg) {
    tester_args *ta = (tester_args *)arg;
    for (int i = 0; i < ta->iters; i++) {
        printf("task %s: %d\n", ta->name, i);
        scheduler_relinquish();
    }
    free(ta);
}

void create_test_task(char *name, int iters) {
    tester_args *ta = malloc(sizeof(*ta));
    ta->name = name;
    ta->iters = iters;
    scheduler_create_task(tester, ta);
}

int main(int argc, char **argv) {
    scheduler_init();
    create_test_task("first", 5);
    create_test_task("second", 2);
    scheduler_run();
    printf("Finished running all tasks!\n");
    return EXIT_SUCCESS;
}
