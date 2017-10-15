# xthread
# Copyright (c) 2017 myxxxsquared https://github.com/myxxxsquared
# Distributed under MIT License

IDIR = .
CC = gcc
CFLAGS = -I$(IDIR) -Ilibc/include -O2 -ggdb -Wall -std=gnu11
LD = gcc
LDFLAGS = 

ODIR=obj
_DEPS = xthread.h _xthread.h
_OBJ = xthread.o xthread_init.o _xthread_lock.o _xthread_schedule.o xthread_thread.o xthread_core.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS) obj/.probe
	$(CC) -c -o $@ $< $(CFLAGS)

bin/xthread: $(OBJ) bin/.probe
	$(LD) -o $@ $(OBJ) $(CFLAGS) $(LIBS) $(LDFLAGS)

obj/.probe :
	mkdir -p $(dir $@) && touch $@

bin/.probe :
	mkdir -p $(dir $@) && touch $@

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o bin/xthread
