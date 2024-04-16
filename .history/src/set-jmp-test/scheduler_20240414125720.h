#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <setjmp.h>

#define STACK_SIZE 16384  // Define an appropriate stack size

typedef struct sc_list_head {
    struct sc_list_head *next, *prev;
} sc_list_head;

typedef struct task {
    enum {
        ST_CREATED,
        ST_RUNNING,
        ST_WAITING,
    } status;

    int id;

    jmp_buf buf;

    void (*func)(void*);
    void *arg;

    struct sc_list_head task_list;

    void *stack_bottom;
    void *stack_top;
    int stack_size;
}; task;

void scheduler_init(void);
void scheduler_create_task(void (*func)(void*), void *arg);
void scheduler_run(void);
void scheduler_relinquish(void);
void scheduler_exit_current_task(void);

#endif // SCHEDULER_H
