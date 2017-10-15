/* xthread */
/* Copyright (c) 2017 myxxxsquared https://github.com/myxxxsquared */
/* Distributed under MIT License */

#include "_xthread.h"

struct xthread_struct* _xthread_fetch_ready()
{
    struct xthread_struct *result, *root, *next;
    _xthread_waitlock(XTHREAD_LOCK_READYQUEUE);

    root = xthread_global.queue_ready;
    if((result=root->queue_next) == root)
    {
        result = 0;
    }
    else
    {
        next = result->queue_next;
        root->queue_next = next;
        next->queue_previous = root;
    }

    _xthread_unlock(XTHREAD_LOCK_READYQUEUE);
    return result;
}

void _xthread_push_ready(struct xthread_struct* t)
{
    struct xthread_struct *root, *back;
    _xthread_waitlock(XTHREAD_LOCK_READYQUEUE);

    root = xthread_global.queue_ready;
    back = root->queue_previous;
    back->queue_next = t;
    root->queue_previous = t;
    t->queue_next = root;
    t->queue_previous = back;

    _xthread_unlock(XTHREAD_LOCK_READYQUEUE);
}

asm(
    ".text\n"
    ".type _xthread_switch_to,@function\n"
    "_xthread_switch_to:\n"
    "   pushq %rbx;\n"
    "   pushq %r12;\n"
    "   pushq %r13;\n"
    "   pushq %r14;\n"
    "   pushq %r15;\n"
    "   pushq %rbp;\n"
    "   \n"
    "   movq %rsp, %rax;\n"
    "   movq %rax, (%rsi);\n"
    "   movq %rdi, %rax;\n"
    "   movq %rax, %rsp;\n"
    "   \n"
    "   popq %rbp;\n"
    "   popq %r15;\n"
    "   popq %r14;\n"
    "   popq %r13;\n"
    "   popq %r12;\n"
    "   popq %rbx;\n"
    "   retq;\n"
);

void _xthread_switch(
    struct xthread_kernel_struct* k,
    struct xthread_struct* t
) {
    k->thread = t;
    k->tid = t->thread;
    _xthread_switch_to((long)t->rsp, (long*)&k->rsp);
    k->thread = 0;
    k->tid = -1;
}

static void* _push_stack(long val, void* rsp)
{
    long* r = (long*)rsp;
    r--;
    *r = val;
    return (void*)r;
}

void _xthread_thread_entry()
{
    struct xthread_kernel_struct* kself = _xthread_kernel_self();
    struct xthread_struct* thread = kself->thread;
    xthread_exit(thread->func(thread->arg));
    _xthread_die("Unreachable code reached.", 0);
    while(1);
}

void _xthread_switch_new(
    struct xthread_kernel_struct* k,
    struct xthread_struct* t
) {
    t->status = XTHREAD_STATUS_READY;
    t->rsp = t->stack + XTHREAD_STACK_SIZE;
    t->rsp = _push_stack((long)&_xthread_thread_entry, t->rsp);
    t->rsp = _push_stack(0, t->rsp);
    t->rsp = _push_stack(0, t->rsp);
    t->rsp = _push_stack(0, t->rsp);
    t->rsp = _push_stack(0, t->rsp);
    t->rsp = _push_stack(0, t->rsp);
    t->rsp = _push_stack(0, t->rsp);
    _xthread_switch(k, t);
}

void _xthread_kernel_run(struct xthread_kernel_struct* thread)
{
    struct xthread_struct* current;
    while(1)
    {
        while(!(current = _xthread_fetch_ready()))
            _xthread_wait();

        switch(current->status)
        {
        case XTHREAD_STATUS_NEW:
            _xthread_switch_new(thread, current);
            break;
        case XTHREAD_STATUS_READY:
            _xthread_switch(thread, current);
            break;
        default:
            _xthread_die("invaild status of thread", current->status);
        }

        switch(current->status)
        {
        case XTHREAD_STATUS_READY:
            _xthread_push_ready(current);
            break;
        case XTHREAD_STATUS_TERIMATED:
            _xthread_destroy(current);
            break;
        default:
            _xthread_die("invaild status of thread", current->status);
        }
    }
}

struct xthread_kernel_struct* _xthread_kernel_self(void)
{
    long result;
    asm(
        "movq %%gs:0, %0\n"
        : "=a"(result)
    );
    return (struct xthread_kernel_struct*)result;
}

long xthread_yield()
{
    struct xthread_kernel_struct* kself = _xthread_kernel_self();
    struct xthread_struct* t = kself->thread;

    _xthread_switch_to((long)kself->rsp, (long*)&t->rsp);
    return 0;
}

long xthread_exit(void* value)
{
    struct xthread_kernel_struct* kself = _xthread_kernel_self();
    kself->thread->status = XTHREAD_STATUS_TERIMATED;
    return xthread_yield();
}

xthread_t xthread_self(void)
{
    struct xthread_kernel_struct* kself = _xthread_kernel_self();
    return kself->tid;
}