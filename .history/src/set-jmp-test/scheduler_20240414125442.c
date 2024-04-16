#include "scheduler.h"
#include <stdlib.h>
#include <stdio.h>

struct scheduler_private {
    jmp_buf buf;
    struct task *current;
    struct sc_list_head task_list;
} priv

void sc_list_init(sc_list_head *list) {
    list->next = list->prev = list;
}

void sc_list_insert_end(sc_list_head *list, sc_list_head *new) {
    new->prev = list->prev;
    new->next = list;
    list->prev->next = new;
    list->prev = new;
}

void sc_list_remove(sc_list_head *entry) {
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
    entry->next = entry->prev = NULL;
}

void scheduler_init(void)
{
    priv.current = NULL;
    sc_list_init(&priv.task_list);
}

void scheduler_create_task(void (*func)(void *), void *arg)
{
    static int id = 1;
    struct task *task = malloc(sizeof(*task));
    task->status = ST_CREATED;
    task->func = func;
    task->arg = arg;
    task->id = id++;
    task->stack_size = 16 * 1024;
    task->stack_bottom = malloc(task->stack_size);
    task->stack_top = task->stack_bottom + task->stack_size;
    sc_list_insert_end(&priv.task_list, &task->task_list);
}

static void scheduler_free_current_task(void) {
    task *t = priv.current;
    priv.current = NULL;
    free(t->stack_bottom);
    free(t);
}

static struct task *scheduler_choose_task(void)
{
    struct task *task;

    sc_list_for_each_entry(task, &priv.task_list, task_list, struct task)
    {
        if (task->status == ST_RUNNING || task->status == ST_CREATED) {
            sc_list_remove(&task->task_list);
            sc_list_insert_end(&priv.task_list, &task->task_list);
            return task;
        }
    }

    return NULL;
}

enum {
    INIT=0,
    SCHEDULE,
    EXIT_TASK,
};

void scheduler_run(void)
{
    /* This is the exit path for the scheduler! */
    switch (setjmp(priv.buf)) {
    case EXIT_TASK:
        scheduler_free_current_task();
    case INIT:
    case SCHEDULE:
        schedule();
        /* if return, there's nothing else to do and we exit */
        return;
    default:
        fprintf(stderr, "Uh oh, scheduler error\n");
        return;
    }
}

static void schedule(void)
{
    struct task *next = scheduler_choose_task();

    if (!next) {
        return;
    }

    priv.current = next;
    if (next->status == ST_CREATED) {
        /*
         * This task has not been started yet. Assign a new stack
         * pointer, run the task, and exit it at the end.
         */
        register void *top = next->stack_top;
        asm volatile(
            "mov %[rs], %%rsp \n"
            : [ rs ] "+r" (top) ::
        );

        /*
         * Run the task function
         */
        next->status = ST_RUNNING;
        next->func(next->arg);

        /*
         * The stack pointer should be back where we set it. Returning would be
         * a very, very bad idea. Let's instead exit
         */
        scheduler_exit_current_task();
    } else {
        longjmp(next->buf, 1);
    }
    /* NO RETURN */
}

void scheduler_relinquish(void)
{
    if (setjmp(priv.current->buf)) {
        return;
    } else {
        longjmp(priv.buf, SCHEDULE);
    }
}

void scheduler_exit_current_task(void) {
    sc_list_remove(&priv.current->task_list);
    longjmp(priv.buf, 2);
}
