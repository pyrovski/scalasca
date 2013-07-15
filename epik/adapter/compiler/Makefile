#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

# mandatory COMP special compiler interface (may be generic)
# optional EXTRAOBJS for that compiler interface

# needs EPIK/config/pomp2_fwrappers_def.h (generated)
# needs EPIK/episode/esd_def.h (uses elg_defs.h)
# needs EPIK/episode/esd_run.h (uses elg_defs.h)

CFLAGS += -I../../config -I../../utils -I../../episode
CFLAGS += $(DEMANGLE)

USER_HDRS = epik_user.h epik_user.inc elg_user.h elg_user.inc VT.h VT.inc

.SUFFIXES:  .cpp .c .f

COMP_SER_LIB = libepk.ad.comp.ser.a
COMP_MPI_LIB = libepk.ad.comp.mpi.a
COMP_OMP_LIB = libepk.ad.comp.omp.a
COMP_HYB_LIB = libepk.ad.comp.ompi.a
COMP_LIB_LIST = $(COMP_SER_LIB) $(COMP_MPI_LIB) $(COMP_OMP_LIB) $(COMP_HYB_LIB)

COMP_SER_OBJS = epk_comp_$(COMP).ser.o epk_comp_vt.o $(EXTRAOBJS)
COMP_MPI_OBJS = epk_comp_$(COMP).mpi.o epk_comp_vt.o $(EXTRAOBJS)
COMP_OMP_OBJS = epk_comp_$(COMP).omp.o epk_comp_vt.o $(EXTRAOBJS)
COMP_HYB_OBJS = epk_comp_$(COMP).ompi.o epk_comp_vt.o $(EXTRAOBJS)

all: $(COMP_SER_LIB) $(MPIELGLIB) $(OMPELGLIB) $(HYBELGLIB) \
        $(OTFMPIELGLIB) $(OTFOMPELGLIB) $(OTFHYBELGLIB)

$(COMP_SER_LIB) : $(COMP_SER_OBJS)
	@$(MAKE) COMP=generic epk_comp_generic.ser.o
	$(C_AR) $(AFLAG) -rcs $@ epk_comp_generic.ser.o $(COMP_SER_OBJS)
	@echo "*** $@ built"

$(MPIELGLIB) $(OTFMPIELGLIB) : $(COMP_MPI_LIB)
$(COMP_MPI_LIB) : $(COMP_MPI_OBJS)
	@$(MAKE) COMP=generic epk_comp_generic.mpi.o
	$(C_AR) $(AFLAG) -rcs $@ epk_comp_generic.mpi.o $(COMP_MPI_OBJS)
	@echo "*** $@ built"

$(OMPELGLIB) $(OTFOMPELGLIB) : $(COMP_OMP_LIB)
$(COMP_OMP_LIB) : $(COMP_OMP_OBJS)
	@$(MAKE) COMP=generic epk_comp_generic.omp.o
	$(C_AR) $(AFLAG) -rcs $@ epk_comp_generic.omp.o $(COMP_OMP_OBJS)
	@echo "*** $@ built"

$(HYBELGLIB) $(OTFHYBELGLIB) : $(COMP_HYB_LIB)
$(COMP_HYB_LIB) : $(COMP_HYB_OBJS)
	@$(MAKE) COMP=generic epk_comp_generic.ompi.o
	$(C_AR) $(AFLAG) -rcs $@ epk_comp_generic.ompi.o $(COMP_HYB_OBJS)
	@echo "*** $@ built"

epk_comp_${COMP}.ser.o : epk_comp_$(COMP).c epk_comp_fgeneric.c
	$(CC) $(CFLAGS) -c ${@:.ser.o=.c} -o $@
epk_comp_${COMP}.mpi.o : epk_comp_$(COMP).c epk_comp_fgeneric.c
	$(MPICC) $(CFLAGS) -DELG_MPI -c ${@:.mpi.o=.c} -o $@
epk_comp_$(COMP).omp.o : epk_comp_$(COMP).c epk_comp_fgeneric.c
	$(OMPCC) $(CFLAGS) -DELG_OMP $(OMPFLAG) -c ${@:.omp.o=.c} -o $@
epk_comp_$(COMP).ompi.o : epk_comp_$(COMP).c epk_comp_fgeneric.c
	$(HYBCC) $(CFLAGS) -DELG_OMPI $(OMPFLAG) -c ${@:.ompi.o=.c} -o $@

epk_comp_vt.o: epk_comp_vt.c epk_comp_fvt.c
	$(CC) $(CFLAGS) -c epk_comp_vt.c

epk_comp_ftrace_1.o : epk_comp_ftrace_1.s
	$(SX)as epk_comp_ftrace_1.s

install: all
	$(MKDIR) -m 755 $(INCDIR) $(LIBDIR)
	@echo "COMP_LIB_LIST=<$(COMP_LIB_LIST)>"
	@list='$(USER_HDRS)'; for p in $$list; do \
		echo "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
		eval "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
	done
	@list='$(COMP_LIB_LIST)'; for p in $$list; do \
            if test -f $$p ; then \
		echo "$(INSTALL) -c -m 644 $$p $(LIBDIR)" ; \
		eval "$(INSTALL) -c -m 644 $$p $(LIBDIR)" ; \
            fi \
	done

clean:
	rm -f *.o lib*.a
