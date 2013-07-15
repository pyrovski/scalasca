#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

# EPILOG base (non-runtime) trace-handling modules
BASE_OBJS = elg_rw.o elg_readcb.o

# elg_defs.h elg_impl.h
CFLAGS += -I../../config -I../../utils -I../../episode

LIB = elg.base
B_LIB = lib$(LIB).a

USER_HDRS = elg_rw.h elg_readcb.h

all: $(B_LIB)

$(B_LIB) : $(BASE_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(BASE_OBJS)
	@echo "*** $(B_LIB) built"

elg_rw.o : elg_rw.c elg_rw.h elg_rw.c.gen
	$(CC) $(CFLAGS) $(SZLIB_CFLAGS) $(SIONLIB_CFLAGS_SER) -c elg_rw.c
elg_readcb.o : elg_readcb.c elg_readcb.h elg_rw.h elg_readcb.c.gen
	$(CC) $(CFLAGS) -c elg_readcb.c

install: $(B_LIB)
	$(MKDIR) -m 755 $(LIBDIR) $(INCDIR)
	$(INSTALL) -c -m 644 $(B_LIB) $(LIBDIR)
	@list='$(USER_HDRS)'; for p in $$list; do \
		echo "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
		eval "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
	done

clean:
	rm -f *.o $(B_LIB)
