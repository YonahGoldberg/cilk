#include "scheduler.h"
#include <stdlib.h>
#include <stdio.h>

static struct {
    jmp_buf buf;
    task *current;
    sc_list_head task_list;
} priv;

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

void scheduler_init(void) {
    priv.current = NULL;
    sc_list_init(&priv.task_list);
}

void scheduler_create_task(void (*func)(void *), void *arg) {
    static int id = 1;
    task *new_task = malloc(sizeof(task));
    new_task->status = ST_CREATED;
    new_task->func = func;
    new_task->arg = arg;
    new_task->id = id++;
    new_task->stack_size = STACK_SIZE;
    new_task->stack_bottom = malloc(new_task->stack_size);
    new_task->stack_top = new_task->stack_bottom + new_task->stack_size;
    sc_list_insert_end(&priv.task_list, &new_task->task_list);
}

static void scheduler_free_current_task(void) {
    task *t = priv.current;
    priv.current = NULL;
    free(t->stack_bottom);
    free(t);
}

static task *scheduler_choose_task(void) {
    sc_list_head *pos;
    for (pos = priv.task_list.next; pos != &priv.task_list; pos = pos->next) {
        task *t = (task *)((char *)pos - offsetof(task, task_list));
        if (t->status == ST_RUNNING || t->status == ST_CREATED) {
            return t;
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

void scheduler_relinquish(void) {
    if (setjmp(priv.current->buf)) {
        return;
    } else {
        longjmp(priv.buf, 1);
    }
}

void scheduler_exit_current_task(void) {
    sc_list_remove(&priv.current->task_list);
    longjmp(priv.buf, 2);
}
