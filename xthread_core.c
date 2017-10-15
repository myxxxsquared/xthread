/* xthread */
/* Copyright (c) 2017 myxxxsquared https://github.com/myxxxsquared */
/* Distributed under MIT License */

#include "_xthread.h"

struct _xthread_global xthread_global;

void _xthread_die(const char* info, long e)
{
    printf("0lx%lx:%s\n",e, info);
    *(char*)0 = 0;
}