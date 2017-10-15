/* xthread */
/* Copyright (c) 2017 myxxxsquared https://github.com/myxxxsquared */
/* Distributed under MIT License */

#include "_xthread.h"

int _xthread_clone_entry(void* arg)
{
    struct xthread_kernel_struct* current = (struct xthread_kernel_struct*)arg;

    _xthread_waitunlock(XTHREAD_LOCK_READY);

    arch_prctl(ARCH_SET_GS, (long)arg);

    _xthread_waitlock(XTHREAD_LOCK_GLIBC);
    printf("Thread %ld inited.\n", current->id);
    _xthread_unlock(XTHREAD_LOCK_GLIBC);

    sleep(1);

    _xthread_kernel_run(current);

    _xthread_die("Unreachable code reached.", 0);
    while(1);
    return 0;
}

void _xthread_create_kernel_thread(long i)
{
    struct xthread_kernel_struct* kthread;
    kthread = xthread_global.kthreads + i;

    kthread->current = kthread;
    kthread->id = i;
    kthread->stack = malloc(XTHREAD_KERNEL_STACK_SIZE);

    kthread->tid = clone(
        &_xthread_clone_entry,
        (void*)((long)kthread->stack + XTHREAD_KERNEL_STACK_SIZE),
        CLONE_FILES | CLONE_FS | CLONE_IO | CLONE_SIGHAND | CLONE_SYSVSEM | CLONE_THREAD | CLONE_VM /* | CLONE_SETTLS */,
        (void*)kthread /* , 0, 0, (void*)kthread */);

    if(-1 == kthread->tid)
    {
        _xthread_die("xthread_init: clone failed.\n", errno);
    }
}

long xthread_init(long nthreads)
{
    if(nthreads <= 0 || nthreads > XTHREAD_MAX_THREAD)
        return EINVAL;

    xthread_global.nthreads = nthreads;
    xthread_global.currentid = 0;
    memset((void*)(xthread_global.xthread_locks), 0, 16);

    struct xthread_struct* xs;
    xs = malloc(sizeof(struct xthread_struct));
    xs->thread = -1;
    xs->previous = xs;
    xs->next = xs;
    xthread_global.xthreads = xs;

    struct xthread_struct* rxs;
    rxs = malloc(sizeof(struct xthread_struct));
    rxs->thread = -1;
    rxs->queue_previous = rxs;
    rxs->queue_next = rxs;
    xthread_global.queue_ready = rxs;

    xthread_global.kthreads = (struct xthread_kernel_struct*)malloc(nthreads * sizeof(struct xthread_kernel_struct));

    _xthread_lock(XTHREAD_LOCK_READY);

    for(long i = 0; i < nthreads; i++)
    {
        _xthread_create_kernel_thread(i);
    }

    _xthread_unlock(XTHREAD_LOCK_READY);
    return 0;
}