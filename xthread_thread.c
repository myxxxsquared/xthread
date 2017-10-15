/* xthread */
/* Copyright (c) 2017 myxxxsquared https://github.com/myxxxsquared */
/* Distributed under MIT License */

#include "_xthread.h"

void _xthread_destroy(struct xthread_struct* thread)
{
    _xthread_waitlock(XTHREAD_LOCK_THREADLIST);
    struct xthread_struct *previous, *next;
    previous = thread->previous;
    next = thread->next;
    previous->next = next;
    next->previous = previous;
    _xthread_unlock(XTHREAD_LOCK_THREADLIST);

    _xthread_waitlock(XTHREAD_LOCK_GLIBC);
    free(thread->stack);
    free(thread);
    _xthread_unlock(XTHREAD_LOCK_GLIBC);
}

long xthread_create(xthread_t* o_threadid, void *(*func)(void*), void* arg)
{
    struct xthread_struct* thread;
    _xthread_waitlock(XTHREAD_LOCK_GLIBC);
    thread = (struct xthread_struct*)malloc(sizeof(struct xthread_struct));
    _xthread_unlock(XTHREAD_LOCK_GLIBC);

    _xthread_waitlock(XTHREAD_LOCK_CURRENTID);
    thread->thread = xthread_global.currentid;
    xthread_global.currentid++;
    _xthread_unlock(XTHREAD_LOCK_CURRENTID);

    thread->flags = 0;
    thread->func = func;
    thread->arg = arg;
    thread->status = XTHREAD_STATUS_NEW;
    _xthread_waitlock(XTHREAD_LOCK_GLIBC);
    thread->stack = malloc(XTHREAD_STACK_SIZE);
    _xthread_unlock(XTHREAD_LOCK_GLIBC);
    thread->rsp = 0;

    _xthread_waitlock(XTHREAD_LOCK_THREADLIST);
    struct xthread_struct *root, *back;
    root = xthread_global.xthreads;
    back = root->previous;
    back->next = thread;
    root->previous = thread;
    thread->previous = back;
    thread->next = root;
    _xthread_unlock(XTHREAD_LOCK_THREADLIST);

    if(o_threadid)
        *o_threadid = thread->thread;

    _xthread_push_ready(thread);

    return 0;
}
