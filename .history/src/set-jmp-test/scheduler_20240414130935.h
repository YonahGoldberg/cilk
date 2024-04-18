#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <setjmp.h>

void scheduler_init(void);
void scheduler_create_task(void (*func)(void*), void *arg);
void scheduler_run(void);
void scheduler_relinquish(void);
void scheduler_exit_current_task(void);

#endif // SCHEDULER_H