#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

ZLIB_LDFLAGS = -lz

CC = gcc
CFLAGS = -g -O2

#------------------------------------------------------------------------------
# Do not change below
#------------------------------------------------------------------------------

.SUFFIXES:  .cpp .c .f

LIB = ../lib

TOOLS = elg_phy_print elg_log_print $(ELG_VTF3) $(ELG_OTF)

all: tools

tools: $(LIB)/elg_types.h $(LIB)/libelg.base.a $(TOOLS)

elg_conv.o: elg_conv.c
	$(CC) $(CFLAGS) -I. -I$(LIB) -c elg_conv.c
elg_phy_print: elg_phy_print.c elg_print.c.gen
	$(CC) $(CFLAGS) -o $@ -I$(LIB) elg_phy_print.c -L$(LIB) -lelg.base $(ZLIB_LDFLAGS)
elg_log_print: elg_log_print.c elg_conv.o
	$(CC) $(CFLAGS) -I. -I$(LIB) elg_log_print.c elg_conv.o \
	-o $@ -L$(LIB) -lelg.base $(ZLIB_LDFLAGS)

#------------------------------------------------------------------------------
# Cleanup
#------------------------------------------------------------------------------

clean:
	rm -f a.out core *.o
	rm -f elg_log_print elg_phy_print
