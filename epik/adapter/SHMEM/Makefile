#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

.SUFFIXES:  .cpp .c .f

CFLAGS += -I../../utils -I../../episode -I../../epilog/rts

# SHMEM-specific adapter modules
SHMEM_OBJS = epk_shmem.o epk_fshmem.o

LIB = epk.ad.shmem
A_LIB = lib$(LIB).a

all: $(A_LIB)

$(A_LIB) : $(SHMEM_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(SHMEM_OBJS)
	@echo "*** $(A_LIB) built"

epk_shmem.o : epk_shmem.c epk_shmem.h epk_shmem.c.reg1.gen \
	epk_shmem.c.reg2.gen epk_shmem.c.wrapper.gen
	$(CC) $(CFLAGS) $(SHMEMINC) $(SHMEMSYS) -c epk_shmem.c

epk_fshmem.o : epk_fshmem.c epk_shmem.h epk_shmem.c.reg1.gen \
	epk_fshmem.c.wrapper.gen
	$(CC) $(CFLAGS) $(SHMEMSYS) -c epk_fshmem.c

install: all
	$(MKDIR) -m 755 $(LIBDIR)
	$(INSTALL) -c -m 644 $(A_LIB) $(LIBDIR)

clean:
	rm -f *.o $(A_LIB)
