/* xthread */
/* Copyright (c) 2017 myxxxsquared https://github.com/myxxxsquared */
/* Distributed under MIT License */

typedef long xthread_t;
struct xthread_struct;
struct xthread_kernel_struct;
struct _xthread_global;

long xthread_init(long nthreads);
long xthread_create(xthread_t* thread, void *(*func)(void*), void* arg);
long xthread_join(xthread_t thread, void** value_ptr);
long xthread_exit(void* value);
long xthread_yield();
xthread_t xthread_self(void);

#define XTHREAD_STATUS_NEW 0
#define XTHREAD_STATUS_READY 1
#define XTHREAD_STATUS_RUNNING 2
#define XTHREAD_STATUS_TERIMATED 3
#define XTHREAD_STATUS_WAIT_RESULT 4

#define XTHREAD_MAX_THREAD 65536

#define XTHREAD_KERNEL_STACK_SIZE 8192
#define XTHREAD_STACK_SIZE 1048576