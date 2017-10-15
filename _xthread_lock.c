/* xthread */
/* Copyright (c) 2017 myxxxsquared https://github.com/myxxxsquared */
/* Distributed under MIT License */

#include "_xthread.h"

void _xthread_wait()
{
    sleep(0);
}

long _xthread_lock(long lock)
{
    char bit;

    asm volatile("xorw %%ax, %%ax\n"
        "incb %%ah\n"
        "lock; cmpxchgb %%ah, (%1)\n"
        "movb %%al, %0"
        : "=r" (bit)
        : "r" (xthread_global.xthread_locks + lock)
        : "memory", "ax");

    return !bit;
}

void _xthread_waitlock(long lock)
{
    while(!_xthread_lock(lock))
        _xthread_wait();
}

void _xthread_unlock(long lock)
{
    xthread_global.xthread_locks[lock] = 0;
}

void _xthread_waitunlock(long lock)
{
    _xthread_waitlock(lock);
    _xthread_unlock(lock);
}