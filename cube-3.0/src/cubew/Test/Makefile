TOPDIR=../../..

####### Compiler, tools and options
include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

CUBEWDIR = ..

CFLAGS = -g -I$(CUBEWDIR)/include/cube3
LDFLAGS = -L$(CUBEWDIR)/lib -lcubew3

CFLAGS = -g -I..
LDFLAGS = ../libcubew3.a

all: cube_test

cube_test: cube_test.c
	$(CC) -o cube_test $(CFLAGS) cube_test.c $(LDFLAGS)

clean:
	rm -f cube_test *.o example.cube
