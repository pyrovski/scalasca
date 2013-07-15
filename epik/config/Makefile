#------------------------------------------------------------------------------
# General Settings
#------------------------------------------------------------------------------

TOPDIR = ../..

include $(TOPDIR)/Makefile.defs
include $(TOPDIR)/mf/common.defs

## Script for execution on (back-end) target platform, only required if
## PLAT not one of those with provided files in directories files-$PLAT.
## Alternatively, run "./gen_headers" manually and store in files-$PLAT.
#REMEXEC = insert-script-here-which-executes-a-program-on-compute-node

ifneq ("$(PLAT)","crayxt")
LIBC = -lc
endif
# NEC SX needs MPI library when linking
ifeq ("$(MPIF90)","sxmpif90")
LIBC = -lmpi
endif

ifneq ("$(MPICC)","")
MPI_TARGETS = epk_defs_mpi.h
endif

.SUFFIXES:  .cpp .c .f

USER_headers = elg_types.h 

all: elg_types.h epk_fwrapper_def.h $(MPI_TARGETS)

gen_elg_types.o : gen_elg_types.c
	$(CC) $(CFLAGS) -c gen_elg_types.c

foos.o : foos.c
	$(CC) $(CFLAGS) -c foos.c

ifeq ("x$(F77)", "x")
gen_headers : gen_headers.c foos.o gen_elg_types.o
	$(CC) $(CFLAGS) foos.o gen_elg_types.o gen_headers.c \
		-o gen_headers
else
gen_headers : gen_headers.f foos.o gen_elg_types.o
	$(F77) $(FFLAGS) foos.o gen_elg_types.o gen_headers.f \
		-o gen_headers $(LIBC)
endif

ifeq ("x$(MPIF90)", "x")
mpi_gen_headers :
	cp files-dummies/elg_fmpi_defs.h .
else
mpi_gen_headers : mpi_gen_headers.f90 foos.o gen_elg_types.o
	$(MPIF90) $(FFLAGS) -c mpi_gen_headers.f90
	$(F90) $(FFLAGS) foos.o gen_elg_types.o mpi_gen_headers.o \
		-o mpi_gen_headers $(LIBC) 
	cp mpi_gen_headers gen_headers
endif

elg_types.h epk_fwrapper_def.h elg_fmpi_defs.h: gen_headers $(MPI_CONFIG_TARGETS)
	if test -d files-$(PLAT); then \
		cp files-$(PLAT)/*.h .; \
	 else \
		$(REMEXEC) ./gen_headers; \
	 fi

epk_defs_mpi.h :
	cp $(TOPDIR)/../epk_defs_mpi.h .

install: all
	$(MKDIR) -m 755 $(INCDIR)
	@list='$(USER_headers)'; for p in $$list; do \
		echo "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
		eval "$(INSTALL) -c -m 644 $$p $(INCDIR)" ; \
	done

clean:
	rm -f elg_types.h epk_fwrapper_def.h elg_fmpi_defs.h epk_defs_mpi.h
	rm -f *.o gen_headers mpi_gen_headers
