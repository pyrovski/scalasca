#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

# mandatory REM_EXEC script for execution on (remote) target platform
# mandatory PLAT specification of target platform
# optional PLAT_CONF description of target platform
# optional METRCFLAGS to enable counter metrics
# optional SZLIB_CFLAGS to enable file compression

.SUFFIXES:  .cpp .c .f

# EPIK utility modules
UTIL_OBJS   = epk_archive.o \
              epk_conf.o \
	      elg_error.o \
              epk_hashtab.o \
              epk_idmap.o \
              epk_memory.o \
	      epk_metmap.o \
              epk_vector.o

LIB   = epk.util
A_LIB = lib$(LIB).a

TOOLS = epik_conf

USER_HDRS = epk_conf.h elg_error.h

all: $(A_LIB) $(TOOLS)

epik_conf: $(A_LIB) epk_conf.c epk_conf.h
	mv epk_conf.o epk_conf_lib.o
	$(CC) $(LDFLAGS) $(SFLAG) $(SIONLIB_CFLAGS_SER) -DSTANDALONE -o $@ epk_conf.c -L. -l$(LIB)
	mv epk_conf_lib.o epk_conf.o

$(A_LIB): $(UTIL_OBJS)
	$(C_AR) $(AFLAG) -rcs $@ $(UTIL_OBJS)
	@echo "*** $(A_LIB) built"

# EPIK/util library objects
epk_archive.o : epk_archive.c epk_archive.h elg_error.h epk_conf.h
	$(CC) $(CFLAGS) -c epk_archive.c
epk_conf.o : epk_conf.c epk_conf.h elg_error.h
	$(PLATCC) $(PLAT_CFLAGS) $(CFLAGS) $(METRCFLAGS) $(SZLIB_CFLAGS) $(SIONLIB_CFLAGS_SER) -c epk_conf.c $(PLAT_CONF) -DDOCDIR=\"$(DOCDIR)\"
elg_error.o : elg_error.c elg_error.h epk_conf.h
	$(CC) $(CFLAGS) -c elg_error.c
epk_hashtab.o: epk_hashtab.c epk_hashtab.h elg_error.h
	$(CC) $(CFLAGS) -c epk_hashtab.c
epk_idmap.o: epk_idmap.c epk_idmap.h
	$(CC) $(CFLAGS) -I. -I../episode -I../config -c epk_idmap.c
epk_metmap.o : epk_metmap.c epk_metric.h elg_error.h epk_conf.h
	$(CC) $(CFLAGS) -I../config -c epk_metmap.c
epk_memory.o : epk_memory.c epk_memory.h
	$(PLATCC) $(PLAT_CFLAGS) $(CFLAGS) -c epk_memory.c
epk_vector.o : epk_vector.c epk_vector.h elg_error.h
	$(CC) $(CFLAGS) -c epk_vector.c

install: all
	$(MKDIR) -m 755 $(BINDIR) $(LIBDIR) $(INCDIR)
	@list='lib*.a'; for p in $$list; do \
		echo "$(INSTALL) -c -m 644 $$p $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 $$p $(LIBDIR)" ; \
	done
	@list='$(TOOLS)'; for p in $$list; do \
		echo "$(INSTALL) -c -m 755 $$p $(BINDIR)" ; \
		eval "$(INSTALL) -c -m 755 $$p $(BINDIR)" ; \
	done
	@list='$(USER_HDRS)'; for p in $$list; do \
		echo "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
		eval "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
	done

clean:
	rm -f *.o $(A_LIB) $(TOOLS)
