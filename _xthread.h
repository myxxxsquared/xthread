/* xthread */
/* Copyright (c) 2017 myxxxsquared https://github.com/myxxxsquared */
/* Distributed under MIT License */

#include "xthread.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <sched.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <asm/prctl.h>
#include <sys/prctl.h>

struct xthread_struct
{
    xthread_t thread;
    long flags;

    void *(*func)(void*);
    void *arg;
    void *result;

    long status;
    void* stack;
    void* rsp;

    struct xthread_struct* previous;
    struct xthread_struct* next;
    struct xthread_struct* queue_previous;
    struct xthread_struct* queue_next;
};

struct xthread_kernel_struct
{
    struct xthread_kernel_struct* current;
    long id;
    long tid;
    struct xthread_struct* thread;
    void* stack;
    void* rsp;
};

struct _xthread_global
{
    char xthread_locks[16];
    long nthreads;
    long currentid;

    struct xthread_kernel_struct* kthreads;
    struct xthread_struct* xthreads;
    struct xthread_struct* queue_ready;
};

void _xthread_wait();
long _xthread_lock(long lock);
void _xthread_waitlock(long lock);
void _xthread_unlock(long lock);
void _xthread_waitunlock(long lock);

int _xthread_clone_entry(void* arg);
void _xthread_create_kernel_thread(long i);
void _xthread_destroy(struct xthread_struct* thread);

struct xthread_struct* _xthread_fetch_ready();
void _xthread_push_ready(struct xthread_struct* t);
void _xthread_thread_entry();
void _xthread_switch(struct xthread_kernel_struct* k, struct xthread_struct* t);
void _xthread_switch_to(long rsp, long* saversp);
void _xthread_switch_new(struct xthread_kernel_struct* k, struct xthread_struct* t);

void _xthread_kernel_run(struct xthread_kernel_struct* thread);
struct xthread_kernel_struct* _xthread_kernel_self(void);

void _xthread_die(const char* info, long e);

extern struct _xthread_global xthread_global;

#define XTHREAD_LOCK_READY 0
#define XTHREAD_LOCK_READYQUEUE 1
#define XTHREAD_LOCK_CURRENTID 2
#define XTHREAD_LOCK_THREADLIST 3
#define XTHREAD_LOCK_GLIBC 4

int arch_prctl(int code, unsigned long addr);