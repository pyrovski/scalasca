#############################################################################
#    Makefile for test sub directory
# 
#    Project: CUBE/src/test
# 
#############################################################################

TOPDIR=../../..
####### Compiler, tools and options
include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

INCPATH	= -I../
LFLAGS	=
LIBS	= -L.. -lcube3
#CCFLAGS = -Wall

####### Files

OBJECTS = cube_test.o

TARGET	= cube_test

####### Implicit rules

.SUFFIXES: .cpp 

.cpp.o:
	$(CCXX) -c $(CCXXFLAGS) $(INCPATH) $<


####### Build rules

all: $(TARGET)

cube_test: cube_test.o ../libcube3.a 
	$(CCXX) $(LDFLAGS) -o cube_test $(OBJECTS) $(LIBS) $(SZLIB_CFLAGS) $(SZLIB_LIBPATH) $(SZLIB_LIB)

clean:
	-rm -f $(OBJECTS) $(TARGET)
	-rm -f *~ core example.cube example2.cube




