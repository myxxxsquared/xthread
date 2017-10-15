/* xthread */
/* Copyright (c) 2017 myxxxsquared https://github.com/myxxxsquared */
/* Distributed under MIT License */

#include "_xthread.h"

void* testfunc(void* arg)
{
    struct xthread_kernel_struct* kself = _xthread_kernel_self();

    for(long i = 1; i <= 5; i++)
    {
        _xthread_waitlock(XTHREAD_LOCK_GLIBC);
        printf("Task %ld is running for the %ld time on Thread %ld.\n",
             xthread_self(), i, kself->id);
        _xthread_unlock(XTHREAD_LOCK_GLIBC);
        for(volatile long j = 0; j < 10000000; j++);
        xthread_yield();
    }
    return 0;
}

int main()
{
    xthread_init(2);

    for(long i = 0; i < 5; ++i)
        xthread_create(0, &testfunc, 0);

    while(1)
        sleep(0);
    return 0;
}